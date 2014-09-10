//
//  EntityTreeRenderer.cpp
//  interface/src
//
//  Created by Brad Hefta-Gaub on 12/6/13.
//  Copyright 2013 High Fidelity, Inc.
//
//  Distributed under the Apache License, Version 2.0.
//  See the accompanying file LICENSE or http://www.apache.org/licenses/LICENSE-2.0.html
//

#include <glm/gtx/quaternion.hpp>

#include <FBXReader.h>

#include "InterfaceConfig.h"

#include <BoxEntityItem.h>
#include <ModelEntityItem.h>
#include <PerfStat.h>


#include "Menu.h"
#include "EntityTreeRenderer.h"

#include "RenderableBoxEntityItem.h"
#include "RenderableModelEntityItem.h"
#include "RenderableSphereEntityItem.h"


QThread* EntityTreeRenderer::getMainThread() {
    return Application::getInstance()->getEntities()->thread();
}



EntityTreeRenderer::EntityTreeRenderer() :
    OctreeRenderer() {
    REGISTER_ENTITY_TYPE_WITH_FACTORY(Model, RenderableModelEntityItem::factory)
    REGISTER_ENTITY_TYPE_WITH_FACTORY(Box, RenderableBoxEntityItem::factory)
    REGISTER_ENTITY_TYPE_WITH_FACTORY(Sphere, RenderableSphereEntityItem::factory)
}

EntityTreeRenderer::~EntityTreeRenderer() {
}

void EntityTreeRenderer::clear() {
    OctreeRenderer::clear();
}

void EntityTreeRenderer::init() {
    OctreeRenderer::init();
    static_cast<EntityTree*>(_tree)->setFBXService(this);
}

void EntityTreeRenderer::setTree(Octree* newTree) {
    OctreeRenderer::setTree(newTree);
    static_cast<EntityTree*>(_tree)->setFBXService(this);
}

void EntityTreeRenderer::update() {
    if (_tree) {
        EntityTree* tree = static_cast<EntityTree*>(_tree);
        tree->update();
    }
}

void EntityTreeRenderer::render(RenderMode renderMode) {
    OctreeRenderer::render(renderMode);
    deleteReleasedModels(); // seems like as good as any other place to do some memory cleanup
}

const FBXGeometry* EntityTreeRenderer::getGeometryForEntity(const EntityItem* entityItem) {
    const FBXGeometry* result = NULL;
    
    if (entityItem->getType() == EntityTypes::Model) {
        const RenderableModelEntityItem* constModelEntityItem = dynamic_cast<const RenderableModelEntityItem*>(entityItem);
        RenderableModelEntityItem* modelEntityItem = const_cast<RenderableModelEntityItem*>(constModelEntityItem);
        assert(modelEntityItem); // we need this!!!
        Model* model = modelEntityItem->getModel(this);
        if (model) {
            result = &model->getGeometry()->getFBXGeometry();
        }
    }
    return result;
}

const Model* EntityTreeRenderer::getModelForEntityItem(const EntityItem* entityItem) {
    const Model* result = NULL;
    if (entityItem->getType() == EntityTypes::Model) {
        const RenderableModelEntityItem* constModelEntityItem = dynamic_cast<const RenderableModelEntityItem*>(entityItem);
        RenderableModelEntityItem* modelEntityItem = const_cast<RenderableModelEntityItem*>(constModelEntityItem);
        assert(modelEntityItem); // we need this!!!
    
        result = modelEntityItem->getModel(this);
    }
    return result;
}

void renderElementProxy(EntityTreeElement* entityTreeElement) {
    glm::vec3 elementCenter = entityTreeElement->getAACube().calcCenter() * (float)TREE_SCALE;
    float elementSize = entityTreeElement->getScale() * (float)TREE_SCALE;
    glColor3f(1.0f, 0.0f, 0.0f);
    glPushMatrix();
        glTranslatef(elementCenter.x, elementCenter.y, elementCenter.z);
        glutWireCube(elementSize);
    glPopMatrix();

    bool displayElementChildProxies = Menu::getInstance()->isOptionChecked(MenuOption::DisplayModelElementChildProxies);

    if (displayElementChildProxies) {
        // draw the children
        float halfSize = elementSize / 2.0f;
        float quarterSize = elementSize / 4.0f;
        glColor3f(1.0f, 1.0f, 0.0f);
        glPushMatrix();
            glTranslatef(elementCenter.x - quarterSize, elementCenter.y - quarterSize, elementCenter.z - quarterSize);
            glutWireCube(halfSize);
        glPopMatrix();

        glColor3f(1.0f, 0.0f, 1.0f);
        glPushMatrix();
            glTranslatef(elementCenter.x + quarterSize, elementCenter.y - quarterSize, elementCenter.z - quarterSize);
            glutWireCube(halfSize);
        glPopMatrix();

        glColor3f(0.0f, 1.0f, 0.0f);
        glPushMatrix();
            glTranslatef(elementCenter.x - quarterSize, elementCenter.y + quarterSize, elementCenter.z - quarterSize);
            glutWireCube(halfSize);
        glPopMatrix();

        glColor3f(0.0f, 0.0f, 1.0f);
        glPushMatrix();
            glTranslatef(elementCenter.x - quarterSize, elementCenter.y - quarterSize, elementCenter.z + quarterSize);
            glutWireCube(halfSize);
        glPopMatrix();

        glColor3f(1.0f, 1.0f, 1.0f);
        glPushMatrix();
            glTranslatef(elementCenter.x + quarterSize, elementCenter.y + quarterSize, elementCenter.z + quarterSize);
            glutWireCube(halfSize);
        glPopMatrix();

        glColor3f(0.0f, 0.5f, 0.5f);
        glPushMatrix();
            glTranslatef(elementCenter.x - quarterSize, elementCenter.y + quarterSize, elementCenter.z + quarterSize);
            glutWireCube(halfSize);
        glPopMatrix();

        glColor3f(0.5f, 0.0f, 0.0f);
        glPushMatrix();
            glTranslatef(elementCenter.x + quarterSize, elementCenter.y - quarterSize, elementCenter.z + quarterSize);
            glutWireCube(halfSize);
        glPopMatrix();

        glColor3f(0.0f, 0.5f, 0.0f);
        glPushMatrix();
            glTranslatef(elementCenter.x + quarterSize, elementCenter.y + quarterSize, elementCenter.z - quarterSize);
            glutWireCube(halfSize);
        glPopMatrix();
    }
}

float EntityTreeRenderer::distanceToCamera(const glm::vec3& center, const ViewFrustum& viewFrustum) const {
    glm::vec3 temp = viewFrustum.getPosition() - center;
    float distanceToVoxelCenter = sqrtf(glm::dot(temp, temp));
    return distanceToVoxelCenter;
}

// TODO: This could be optimized to be a table, or something that doesn't require recalculation on every 
//       render call for every entity
// TODO: This is essentially the same logic used to render voxels, but since models are more detailed then voxels
//       I've added a voxelToModelRatio that adjusts how much closer to a model you have to be to see it.
bool EntityTreeRenderer::shouldRenderEntity(float largestDimension, float distanceToCamera) const {
    const float voxelToModelRatio = 4.0f; // must be this many times closer to a model than a voxel to see it.
    float voxelSizeScale = Menu::getInstance()->getVoxelSizeScale();
    int boundaryLevelAdjust = Menu::getInstance()->getBoundaryLevelAdjust();
    
    float scale = (float)TREE_SCALE;
    float visibleDistanceAtScale = boundaryDistanceForRenderLevel(boundaryLevelAdjust, voxelSizeScale) / voxelToModelRatio;

    while (scale > largestDimension) {
        scale /= 2.0f;
        visibleDistanceAtScale /= 2.0f;
    }
    
    if (scale < largestDimension) {
        visibleDistanceAtScale *= 2.0f;
    }

    return (distanceToCamera <= visibleDistanceAtScale);
}

void EntityTreeRenderer::renderElement(OctreeElement* element, RenderArgs* args) {
    args->_elementsTouched++;
    // actually render it here...
    // we need to iterate the actual entityItems of the element
    EntityTreeElement* entityTreeElement = static_cast<EntityTreeElement*>(element);

    QList<EntityItem*>& entityItems = entityTreeElement->getEntities();
    

    uint16_t numberOfEntities = entityItems.size();

    bool isShadowMode = args->_renderMode == OctreeRenderer::SHADOW_RENDER_MODE;
    bool displayElementProxy = Menu::getInstance()->isOptionChecked(MenuOption::DisplayModelElementProxy);



    if (!isShadowMode && displayElementProxy && numberOfEntities > 0) {
        renderElementProxy(entityTreeElement);
    }
    
    for (uint16_t i = 0; i < numberOfEntities; i++) {
        EntityItem* entityItem = entityItems[i];
        
        if (entityItem->isVisible()) {
            // render entityItem 
            AACube entityCube = entityItem->getAACube();

            entityCube.scale(TREE_SCALE);
        
            // TODO: some entity types (like lights) might want to be rendered even
            // when they are outside of the view frustum...
            float distance = distanceToCamera(entityCube.calcCenter(), *args->_viewFrustum);
            if (shouldRenderEntity(entityCube.getLargestDimension(), distance) &&
                    args->_viewFrustum->cubeInFrustum(entityCube) != ViewFrustum::OUTSIDE) {

                Glower* glower = NULL;
                if (entityItem->getGlowLevel() > 0.0f) {
                    glower = new Glower(entityItem->getGlowLevel());
                }
                entityItem->render(args);
                if (glower) {
                    delete glower;
                }
            } else {
                args->_itemsOutOfView++;
            }
        }
    }
}

float EntityTreeRenderer::getSizeScale() const { 
    return Menu::getInstance()->getVoxelSizeScale();
}

int EntityTreeRenderer::getBoundaryLevelAdjust() const { 
    return Menu::getInstance()->getBoundaryLevelAdjust();
}


void EntityTreeRenderer::processEraseMessage(const QByteArray& dataByteArray, const SharedNodePointer& sourceNode) {
    static_cast<EntityTree*>(_tree)->processEraseMessage(dataByteArray, sourceNode);
}

Model* EntityTreeRenderer::allocateModel(const QString& url) {
    Model* model = NULL;
    // Make sure we only create and delete models on the thread that owns the EntityTreeRenderer
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "allocateModel", Qt::BlockingQueuedConnection, 
                Q_RETURN_ARG(Model*, model),
                Q_ARG(const QString&, url));

        return model;
    }
    model = new Model();
    model->init();
    model->setURL(QUrl(url));
    return model;
}

Model* EntityTreeRenderer::updateModel(Model* original, const QString& newUrl) {
    Model* model = NULL;

    // The caller shouldn't call us if the URL doesn't need to change. But if they
    // do, we just return their original back to them.
    if (!original || (QUrl(newUrl) == original->getURL())) {
        return original;
    }

    // Before we do any creating or deleting, make sure we're on our renderer thread
    if (QThread::currentThread() != thread()) {
        QMetaObject::invokeMethod(this, "updateModel", Qt::BlockingQueuedConnection, 
                Q_RETURN_ARG(Model*, model),
                Q_ARG(Model*, original),
                Q_ARG(const QString&, newUrl));

        return model;
    }

    // at this point we know we need to replace the model, and we know we're on the
    // correct thread, so we can do all our work.
    if (original) {
        delete original; // delete the old model...
    }

    // create the model and correctly initialize it with the new url
    model = new Model();
    model->init();
    model->setURL(QUrl(newUrl));
        
    return model;
}

void EntityTreeRenderer::releaseModel(Model* model) {
    // If we're not on the renderer's thread, then remember this model to be deleted later
    if (QThread::currentThread() != thread()) {
        _releasedModels << model;
    } else { // otherwise just delete it right away
        delete model;
    }
}

void EntityTreeRenderer::deleteReleasedModels() {
    if (_releasedModels.size() > 0) {
        foreach(Model* model, _releasedModels) {
            delete model;
        }
        _releasedModels.clear();
    }
}



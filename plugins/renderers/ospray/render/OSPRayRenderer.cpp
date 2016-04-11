/* Copyright (c) 2011-2016, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of BRayns
 */

#include <brayns/common/log.h>

#include "OSPRayFrameBuffer.h"
#include "OSPRayRenderer.h"
#include "OSPRayScene.h"
#include "OSPRayCamera.h"

#ifndef __APPLE__
// GCC automtically removes the library if the application does not
// make an explicit use of one of its classes. In the case of OSPRay
// classes are loaded dynamicaly. The following line is only to make
// sure that the hbpKernel library is loaded.
#  include <plugins/renderers/ospray/render/ExtendedOBJRenderer.h>
#  include <plugins/renderers/ospray/render/ProximityRenderer.h>
brayns::ExtendedOBJRenderer extendedObjRenderer;
brayns::ProximityRenderer proximityRenderer;
#endif

namespace brayns
{

OSPRayRenderer::OSPRayRenderer( RenderingParameters& renderingParameters )
    : Renderer( renderingParameters )
{
    if( _renderingParameters.getModule( ) != "" )
        ospLoadModule( _renderingParameters.getModule( ).c_str( ));

    _renderer = ospNewRenderer( _renderingParameters.getRenderer( ).c_str( ));
    assert( _renderer );
}

void OSPRayRenderer::render( FrameBufferPtr frameBuffer )
{
    OSPRayFrameBuffer* osprayFrameBuffer =
        dynamic_cast< OSPRayFrameBuffer* >( frameBuffer.get( ));
    ospRenderFrame(
        osprayFrameBuffer->impl( ), _renderer,
        OSP_FB_COLOR | OSP_FB_DEPTH | OSP_FB_ACCUM );
}

void OSPRayRenderer::commit( )
{
    Vector3f color = _renderingParameters.getBackgroundColor( );
    ospSet3f( _renderer, "bgColor", color.x( ), color.y( ), color.z( ));
    ospSet1i( _renderer, "shadowsEnabled", _renderingParameters.getShadows( ));
    ospSet1i( _renderer, "softShadowsEnabled",
        _renderingParameters.getSoftShadows( ));
    ospSet1f( _renderer, "ambientOcclusionStrength",
        _renderingParameters.getAmbientOcclusionStrength( ));
    ospSet1i( _renderer, "shadingEnabled",
        _renderingParameters.getLightShading( ));
    ospSet1f( _renderer, "timestamp", _scene->getTimestamp( ));
    ospSet1i( _renderer, "randomNumber", rand() % 1000 );
    ospSet1i( _renderer, "spp", _renderingParameters.getSamplesPerPixel( ));
    ospSet1i( _renderer, "electronShading",
        _renderingParameters.getElectronShading( ));
    ospSet1i( _renderer, "gradientBackgroundEnabled",
        _renderingParameters.getGradientBackground( ));
    ospSet1f( _renderer, "epsilon", _renderingParameters.getEpsilon( ));
    ospSet1i( _renderer, "moving", false );
    ospSet1f( _renderer, "detectionDistance",
        _renderingParameters.getDetectionDistance( ));
    ospSet1i( _renderer, "detectionOnDifferentMaterial",
        _renderingParameters.getDetectionOnDifferentMaterial( ));
    color = _renderingParameters.getDetectionNearColor( );
    ospSet3f( _renderer, "detectionNearColor",
        color.x( ), color.y( ), color.z( ));
    color = _renderingParameters.getDetectionFarColor( );
    ospSet3f( _renderer, "detectionFarColor",
        color.x( ), color.y( ), color.z( ));

    OSPRayScene* osprayScene = static_cast< OSPRayScene* >( _scene.get( ));
    assert( osprayScene );
    ospSetObject( _renderer, "world", osprayScene->impl( ));

    ospCommit( _renderer );
}

void OSPRayRenderer::setCamera( CameraPtr camera )
{
    OSPRayCamera* osprayCamera = static_cast<OSPRayCamera*>( camera.get( ));
    assert( osprayCamera );
    ospSetObject( _renderer, "camera", osprayCamera->ospImpl( ));
}

}
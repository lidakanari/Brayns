/* Copyright (c) 2015-2018, EPFL/Blue Brain Project
 * All rights reserved. Do not distribute without permission.
 * Responsible Author: Cyrille Favreau <cyrille.favreau@epfl.ch>
 *
 * This file is part of Brayns <https://github.com/BlueBrain/Brayns>
 *
 * This library is free software; you can redistribute it and/or modify it under
 * the terms of the GNU Lesser General Public License version 3.0 as published
 * by the Free Software Foundation.
 *
 * This library is distributed in the hope that it will be useful, but WITHOUT
 * ANY WARRANTY; without even the implied warranty of MERCHANTABILITY or FITNESS
 * FOR A PARTICULAR PURPOSE.  See the GNU Lesser General Public License for more
 * details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with this library; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "RenderingParameters.h"
#include <brayns/common/exceptions.h>
#include <brayns/common/log.h>

#include <boost/lexical_cast.hpp>

namespace
{
const std::string PARAM_ACCUMULATION = "accumulation";
const std::string PARAM_AMBIENT_OCCLUSION = "ambient-occlusion";
const std::string PARAM_AMBIENT_OCCLUSION_DISTANCE =
    "ambient-occlusion-distance";
const std::string PARAM_BACKGROUND_COLOR = "background-color";
const std::string PARAM_CAMERA = "camera";
const std::string PARAM_DETECTION_DISTANCE = "detection-distance";
const std::string PARAM_DETECTION_FAR_COLOR = "detection-far-color";
const std::string PARAM_DETECTION_NEAR_COLOR = "detection-near-color";
const std::string PARAM_DETECTION_ON_DIFFERENT_MATERIAL =
    "detection-on-different-material";
const std::string PARAM_ENGINE = "engine";
const std::string PARAM_EPSILON = "epsilon";
const std::string PARAM_HEAD_LIGHT = "head-light";
const std::string PARAM_MAX_ACCUMULATION_FRAMES = "max-accumulation-frames";
const std::string PARAM_MODULE = "module";
const std::string PARAM_RADIANCE = "radiance";
const std::string PARAM_RENDERER = "renderer";
const std::string PARAM_SHADING = "shading";
const std::string PARAM_SHADOWS = "shadows";
const std::string PARAM_SOFT_SHADOWS = "soft-shadows";
const std::string PARAM_SPP = "samples-per-pixel";
const std::string PARAM_SPR = "samples-per-ray";
const std::string PARAM_STEREO_MODE = "stereo-mode";
const std::string PARAM_VARIANCE_THRESHOLD = "variance-threshold";

const std::array<std::string, 2> ENGINES = {{"ospray", "optix"}};
const std::array<std::string, 7> RENDERERS = {
    {"default", "proximity", "simulation", "particle", "geometrynormals",
     "shadingnormals", "scientificvisualization"}};

const std::array<std::string, 7> RENDERER_INTERNAL_NAMES = {
    {"basic", "proximityrenderer", "simulationrenderer", "particlerenderer",
     "raycast_Ng", "raycast_Ns", "scivis"}};

const std::array<std::string, 4> CAMERA_TYPE_NAMES = {
    {"perspective", "orthographic", "panoramic", "clipped"}};

const std::array<std::string, 4> STEREO_MODES = {
    {"none", "left", "right", "side-by-side"}};

const std::array<std::string, 3> SHADING_TYPES = {
    {"none", "diffuse", "electron"}};
}

namespace brayns
{
RenderingParameters::RenderingParameters()
    : AbstractParameters("Rendering")
{
    _parameters.add_options()(PARAM_ENGINE.c_str(), po::value<std::string>(),
                              "Engine name [ospray|optix]")(
        PARAM_MODULE.c_str(), po::value<std::string>(),
        "OSPRay module name [string]")(
        PARAM_RENDERER.c_str(), po::value<std::string>(),
        "OSPRay active renderer [default|simulation|proximity|particle]")(
        PARAM_SPP.c_str(), po::value<size_t>(),
        "Number of samples per pixel [int]")(
        PARAM_ACCUMULATION.c_str(), po::value<bool>(),
        "Enable/Disable accumulation [bool]")(
        PARAM_AMBIENT_OCCLUSION.c_str(), po::value<float>(),
        "Ambient occlusion distance [float]")(
        PARAM_AMBIENT_OCCLUSION_DISTANCE.c_str(), po::value<float>(),
        "Ambient occlusion distance [float]")(PARAM_SHADOWS.c_str(),
                                              po::value<float>(),
                                              "Shadows intensity [float]")(
        PARAM_SOFT_SHADOWS.c_str(), po::value<float>(),
        "Soft shadows strength [float]")(
        PARAM_SHADING.c_str(), po::value<std::string>(),
        "Shading type [none|diffuse|electron]")(
        PARAM_RADIANCE.c_str(), po::value<bool>(),
        "Enable/Disable radiance [bool]")(
        PARAM_BACKGROUND_COLOR.c_str(), po::value<floats>()->multitoken(),
        "Background color [float float float]")(
        PARAM_DETECTION_DISTANCE.c_str(), po::value<float>(),
        "Detection distance in model units [float]")(
        PARAM_DETECTION_ON_DIFFERENT_MATERIAL.c_str(), po::value<bool>(),
        "Enable/Disable detection on different materials only [bool]")(
        PARAM_DETECTION_NEAR_COLOR.c_str(), po::value<floats>()->multitoken(),
        "Detection near color [float float float]")(
        PARAM_DETECTION_FAR_COLOR.c_str(), po::value<floats>()->multitoken(),
        "Detection far color [float float float]")(
        PARAM_EPSILON.c_str(), po::value<float>(),
        "All intersection distances less than the "
        "epsilon value are ignored by the ray-tracer [float]")(
        PARAM_CAMERA.c_str(), po::value<std::string>(),
        "Camera [perspective|stereo|orthographic|panoramic]")(
        PARAM_STEREO_MODE.c_str(), po::value<std::string>(),
        "Stereo mode [none|left|right|side-by-side]")(
        PARAM_HEAD_LIGHT.c_str(), po::value<bool>(),
        "Enable/Disable light source attached to camera origin [bool]")(
        PARAM_VARIANCE_THRESHOLD.c_str(), po::value<float>(),
        "Threshold for adaptive accumulation [float]")(PARAM_SPR.c_str(),
                                                       po::value<size_t>(),
                                                       "Samples per ray [int]")(
        PARAM_MAX_ACCUMULATION_FRAMES.c_str(), po::value<size_t>(),
        "Maximum number of accumulation frames");

    initializeDefaultRenderers();
    initializeDefaultCameras();
}

void RenderingParameters::initializeDefaultRenderers()
{
    _rendererNames = {RENDERER_INTERNAL_NAMES.begin(),
                      RENDERER_INTERNAL_NAMES.end()};
    _renderers.clear();
    _renderers.push_back(RendererType::default_);
    _renderers.push_back(RendererType::simulation);
    _renderers.push_back(RendererType::particle);
    _renderers.push_back(RendererType::proximity);
    _renderers.push_back(RendererType::geometryNormals);
    _renderers.push_back(RendererType::shadingNormals);
    _renderers.push_back(RendererType::scientificvisualization);
}

void RenderingParameters::initializeDefaultCameras()
{
    _cameraTypeNames = {CAMERA_TYPE_NAMES.begin(), CAMERA_TYPE_NAMES.end()};
}

void RenderingParameters::parse(const po::variables_map& vm)
{
    if (vm.count(PARAM_ENGINE))
    {
        _engine = EngineType::ospray;
        const std::string& engine = vm[PARAM_ENGINE].as<std::string>();
        for (size_t i = 0; i < sizeof(ENGINES) / sizeof(ENGINES[0]); ++i)
            if (engine == ENGINES[i])
                _engine = static_cast<EngineType>(i);
    }
    if (vm.count(PARAM_MODULE))
        _module = vm[PARAM_MODULE].as<std::string>();
    if (vm.count(PARAM_RENDERER))
    {
        _renderer = RendererType::default_;
        const std::string& rendererName = vm[PARAM_RENDERER].as<std::string>();
        auto it = std::find(_rendererNames.begin(), _rendererNames.end(),
                            rendererName);
        if (it == _rendererNames.end())
        {
            BRAYNS_INFO << "'" << rendererName << "' replaces default renderer"
                        << std::endl;
            _rendererNames[0] = rendererName;
        }
        else
            _renderer = static_cast<RendererType>(
                std::distance(_rendererNames.begin(), it));
    }
    if (vm.count(PARAM_SPP))
        _spp = vm[PARAM_SPP].as<size_t>();
    if (vm.count(PARAM_ACCUMULATION))
        _accumulation = vm[PARAM_ACCUMULATION].as<bool>();
    if (vm.count(PARAM_AMBIENT_OCCLUSION))
        _ambientOcclusionStrength = vm[PARAM_AMBIENT_OCCLUSION].as<float>();
    if (vm.count(PARAM_AMBIENT_OCCLUSION_DISTANCE))
        _ambientOcclusionDistance =
            vm[PARAM_AMBIENT_OCCLUSION_DISTANCE].as<float>();
    if (vm.count(PARAM_SHADOWS))
        _shadows = vm[PARAM_SHADOWS].as<float>();
    if (vm.count(PARAM_SOFT_SHADOWS))
        _softShadows = vm[PARAM_SOFT_SHADOWS].as<float>();
    if (vm.count(PARAM_SHADING))
    {
        _shading = ShadingType::diffuse;
        const std::string& shading = vm[PARAM_SHADING].as<std::string>();
        for (size_t i = 0; i < sizeof(SHADING_TYPES) / sizeof(SHADING_TYPES[0]);
             ++i)
            if (shading == SHADING_TYPES[i])
                _shading = static_cast<ShadingType>(i);
    }
    if (vm.count(PARAM_BACKGROUND_COLOR))
    {
        floats values = vm[PARAM_BACKGROUND_COLOR].as<floats>();
        if (values.size() == 3)
            _backgroundColor = Vector3f(values[0], values[1], values[2]);
    }
    if (vm.count(PARAM_DETECTION_DISTANCE))
        _detectionDistance = vm[PARAM_DETECTION_DISTANCE].as<float>();
    if (vm.count(PARAM_DETECTION_ON_DIFFERENT_MATERIAL))
        _detectionOnDifferentMaterial =
            vm[PARAM_DETECTION_ON_DIFFERENT_MATERIAL].as<bool>();
    if (vm.count(PARAM_DETECTION_NEAR_COLOR))
    {
        floats values = vm[PARAM_DETECTION_NEAR_COLOR].as<floats>();
        if (values.size() == 3)
            _detectionNearColor = Vector3f(values[0], values[1], values[2]);
    }
    if (vm.count(PARAM_DETECTION_FAR_COLOR))
    {
        floats values = vm[PARAM_DETECTION_FAR_COLOR].as<floats>();
        if (values.size() == 3)
            _detectionFarColor = Vector3f(values[0], values[1], values[2]);
    }
    if (vm.count(PARAM_EPSILON))
        _epsilon = vm[PARAM_EPSILON].as<float>();
    if (vm.count(PARAM_CAMERA))
    {
        _cameraType = CameraType::default_;
        const std::string& cameraTypeName = vm[PARAM_CAMERA].as<std::string>();
        auto it = std::find(_cameraTypeNames.begin(), _cameraTypeNames.end(),
                            cameraTypeName);
        if (it == _cameraTypeNames.end())
        {
            BRAYNS_INFO << "'" << cameraTypeName << "' replaces default camera"
                        << std::endl;
            _cameraTypeNames[0] = cameraTypeName;
        }
        else
            _cameraType = static_cast<CameraType>(
                std::distance(_cameraTypeNames.begin(), it));
    }
    if (vm.count(PARAM_STEREO_MODE))
    {
        _stereoMode = StereoMode::none;
        const std::string& stereoMode = vm[PARAM_STEREO_MODE].as<std::string>();
        for (size_t i = 0; i < STEREO_MODES.size(); ++i)
            if (stereoMode == STEREO_MODES[i])
                _stereoMode = static_cast<StereoMode>(i);
    }
    if (vm.count(PARAM_HEAD_LIGHT))
        _headLight = vm[PARAM_HEAD_LIGHT].as<bool>();
    if (vm.count(PARAM_VARIANCE_THRESHOLD))
        _varianceThreshold = vm[PARAM_VARIANCE_THRESHOLD].as<float>();
    if (vm.count(PARAM_SPR))
        _spr = vm[PARAM_SPR].as<size_t>();
    if (vm.count(PARAM_MAX_ACCUMULATION_FRAMES))
        _maxAccumFrames = vm[PARAM_MAX_ACCUMULATION_FRAMES].as<size_t>();
    markModified();
}

void RenderingParameters::print()
{
    AbstractParameters::print();
    BRAYNS_INFO << "Engine                            :"
                << getEngineAsString(_engine) << std::endl;
    BRAYNS_INFO << "Module                            :" << _module
                << std::endl;
    BRAYNS_INFO << "Supported renderers               :" << std::endl;
    for (const auto& renderer : _renderers)
        BRAYNS_INFO << "- " << getRendererAsString(renderer) << std::endl;
    BRAYNS_INFO << "Renderer                          :"
                << getRendererAsString(_renderer) << std::endl;
    BRAYNS_INFO << "Samples per pixel                 :" << _spp << std::endl;
    BRAYNS_INFO << "Ambient occlusion                 :" << std::endl;
    BRAYNS_INFO << "- Strength                        :"
                << _ambientOcclusionStrength << std::endl;
    BRAYNS_INFO << "- Distance                        :"
                << _ambientOcclusionDistance << std::endl;
    BRAYNS_INFO << "Shadows                           :" << _shadows
                << std::endl;
    BRAYNS_INFO << "Soft shadows                      :" << _softShadows
                << std::endl;
    BRAYNS_INFO << "Shading                           :"
                << getShadingAsString(_shading) << std::endl;
    BRAYNS_INFO << "Background color                  :" << _backgroundColor
                << std::endl;
    BRAYNS_INFO << "Detection: " << std::endl;
    BRAYNS_INFO << "- Detection distance              : " << _detectionDistance
                << std::endl;
    BRAYNS_INFO << "- Detection on different material : "
                << (_detectionOnDifferentMaterial ? "on" : "off") << std::endl;
    BRAYNS_INFO << "- Detection near color            : " << _detectionNearColor
                << std::endl;
    BRAYNS_INFO << "- Detection far color             : " << _detectionFarColor
                << std::endl;
    BRAYNS_INFO << "Epsilon                           : " << _epsilon
                << std::endl;
    BRAYNS_INFO << "Camera                            : "
                << getCameraTypeAsString(_cameraType) << std::endl;
    BRAYNS_INFO << "Stereo mode                       : "
                << getStereoModeAsString(_stereoMode) << std::endl;
    BRAYNS_INFO << "Accumulation                      : "
                << (_accumulation ? "on" : "off") << std::endl;
    BRAYNS_INFO << "Samples per ray                   : " << _spr << std::endl;
    BRAYNS_INFO << "Max. accumulation frames          : " << _maxAccumFrames
                << std::endl;
}

const std::string& RenderingParameters::getEngineAsString(
    const EngineType value) const
{
    return ENGINES[static_cast<size_t>(value)];
}

const std::string& RenderingParameters::getRendererAsString(
    const RendererType value) const
{
    return _rendererNames[static_cast<size_t>(value)];
}

const std::string& RenderingParameters::getCameraTypeAsString(
    const CameraType value) const
{
    return _cameraTypeNames[static_cast<size_t>(value)];
}

const std::string& RenderingParameters::getStereoModeAsString(
    const StereoMode value) const
{
    return STEREO_MODES[static_cast<size_t>(value)];
}

const std::string& RenderingParameters::getShadingAsString(
    const ShadingType value) const
{
    return SHADING_TYPES[static_cast<size_t>(value)];
}
}

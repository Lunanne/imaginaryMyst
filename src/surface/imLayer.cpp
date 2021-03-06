/* This file is part of imaginaryMyst.
 *
 * imaginaryMyst is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * imaginaryMyst is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with imaginaryMyst.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "imAnimLayer.h"
#include "imCommon.h"
#include "scene/imSceneIndex.h"

/* imColorRGBA */
void imColorRGBA::read(imStream* stream)
{
    m_red = stream->readFloat();
    m_green = stream->readFloat();
    m_blue = stream->readFloat();
    m_alpha = stream->readFloat();
}


/* imLayer */
imLayer* imLayer::MakeLayer(unsigned int type)
{
    if ((type & kLayerAnimation) != 0)
        return new imAnimLayer(type);
    else if ((type & kLayerWater) != 0)
        return new imWaterLayer(type);
    else if ((type & kLayerFire) != 0)
        return new imFireLayer(type);
    else if ((type & kLayerAVI) != 0)
        return new imAVILayer(type);
    else if ((type & kLayerQT) != 0)
        return new imQTLayer(type);
    else if ((type & kLayerBink) != 0)
        return new imBinkLayer(type);
    else
        return new imLayer(type);
}

bool imLayer::read(imStream* stream)
{
    m_blendFlags = stream->read32();
    m_clampFlags = stream->read32();
    m_shadeFlags = stream->read32();
    m_zFlags = stream->read32();
    m_miscFlags = stream->read32();

    imLog("DEBUG: [SDB] [LAYER] (+14) {}", stream->readFloat());
    imLog("DEBUG: [SDB] [LAYER] (+18) {}", stream->readFloat());

    if ((m_zFlags & kZLODBias) != 0)
        m_lodBias = stream->readFloat();
    if ((m_layerType & kLayer_100) != 0)
        imLog("DEBUG: [SDB] [LAYER] (+20) {}", stream->readFloat());
    if ((m_layerType & kLayer_200) != 0)
        imLog("DEBUG: [SDB] [LAYER] (+24) {}", stream->readFloat());
    if ((m_layerType & kLayer_4000) != 0)
        imLog("DEBUG: [SDB] [LAYER] (+28) {}", stream->readFloat());

    m_ambient.read(stream);
    m_color.read(stream);

    if ((m_layerType & kLayer_10) != 0) {
        imLog("DEBUG: [SDB] [LAYER] (TransformMatrix)");
        m_transform.read(stream);
    }

    if ((m_layerType & kLayerAnimBehavior) != 0) {
        if (!m_behavior.read(stream))
            return false;
    }
    if ((m_layerType & kLayerAnimScale) != 0)
        m_animScale.read(stream);
    if ((m_layerType & kLayerAnimAmbient) != 0)
        m_animAmbient.read(stream);
    if ((m_layerType & kLayerAnimColor) != 0)
        m_animColor.read(stream);
    if ((m_layerType & kLayerAnimUVW) != 0)
        m_animUVW.read(stream);

    if ((m_layerType & 0x1000) != 0) {
        imLog("ERROR: Layer flags 0x{X} not currently supported",
              m_layerType & 0x1000);
        return false;
    }

    return true;
}

void imLayer::readTextures(imStream* stream, imSceneIndex* idx)
{
    m_textures.resize(1);
    m_textures[0] = idx->texture(stream->read32());
}

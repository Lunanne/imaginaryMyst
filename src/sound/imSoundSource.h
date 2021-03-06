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

#ifndef _IM_SOUNDSOURCE_H
#define _IM_SOUNDSOURCE_H

#include "imKeyList.h"
#include "imRef.h"

class imSoundSource : public imDataRef {
public:
    imSoundSource()
    { }

    ~imSoundSource()
    { }

    bool read(imStream* stream);

private:
    imKeyList m_keys;
    std::vector<ST::string> m_soundKeys;
    ST::string m_sceneObjectKey, m_animPathKey, m_listenerKey, m_observerKey;
};

#endif

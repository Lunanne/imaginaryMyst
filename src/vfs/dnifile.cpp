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

#include "imVfs.h"
#include "imRef.h"

#define DIRT_MAGIC      0x74726944
#define DIRT_VERSION    0x00010000

struct dni_Context {
    unsigned int m_dirOffset;
    unsigned int m_fileOffset;
    unsigned int m_namesOffset;
    unsigned int m_dataOffset;
    unsigned int m_whatever;
    imStream* m_stream;
    ST::string m_location;
};

static ST::string read_zstring(imStream* stream)
{
    unsigned int offset = stream->read32();
    size_t psave = stream->tell();
    stream->seek(offset);
    ST::string str = stream->readZString();
    stream->seek(psave);
    return str;
}

static imVfsEntry* read_dnidirs(dni_Context* dni, unsigned int offset)
{
    dni->m_stream->seek(offset);
    ST::string name = read_zstring(dni->m_stream);
    if (offset >= dni->m_fileOffset) {
        // File entry
        unsigned int vext = dni->m_stream->read32();
        size_t size = dni->m_stream->read32();
        size_t voffset = dni->m_stream->read32();
        size_t csize = dni->m_stream->read32();
        return new imVfsFileEntry(name, imVfsEntry::Type_VFile, vext, voffset,
                                  size, csize, dni->m_location);
    } else {
        size_t count = dni->m_stream->read32();
        std::list<imRef<imVfsEntry> > children;
        for (size_t i=0; i<count; i++) {
            size_t dirOff = dni->m_stream->read32();
            size_t psave = dni->m_stream->tell();
            children.push_back(read_dnidirs(dni, dirOff));
            dni->m_stream->seek(psave);
        }
        return new imVfsDirEntry(name, children);
    }
}

imVfsEntry* read_dnifile(imStream* stream, const ST::string& location)
{
    if (stream->read32() != DIRT_MAGIC)
        return 0;
    if (stream->read32() != DIRT_VERSION)
        return 0;

    dni_Context dni;
    dni.m_dirOffset = stream->read32();
    dni.m_fileOffset = stream->read32();
    dni.m_namesOffset = stream->read32();
    dni.m_dataOffset = stream->read32();
    dni.m_whatever = stream->read32();
    dni.m_stream = stream;
    dni.m_location = location;
    return read_dnidirs(&dni, dni.m_dirOffset);
}

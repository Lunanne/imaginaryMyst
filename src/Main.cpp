#include "imCommon.h"
#include "scene/imSceneDatabase.h"
#include "surface/imMipmap.h"

#ifdef WIN32
#  include <windows.h>
#  include <direct.h>
#endif

imString s_rootPath;
imVfs s_vfs;
SDL_Surface* s_display;

PFNGLCOMPRESSEDTEXIMAGE2DARBPROC glCompressedTexImage2D_EXT = 0;

#ifdef WIN32
  int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance,
                     LPSTR lpCmdLine, int nCmdShow)
#else
  int main(int argc, char* argv[])
#endif
{
    /* Initialize SDL */
    SDL_Init(SDL_INIT_AUDIO | SDL_INIT_VIDEO);

#ifdef WIN32
    s_logFile = fopen("imaginaryMyst.log", "wb");

    char pathbuf[MAX_PATH];
    s_rootPath = _getcwd(pathbuf, MAX_PATH);
#else
    s_logFile = stderr;
    s_rootPath = path_pathname(argv[0]);
    if (s_rootPath.isEmpty())
        s_rootPath = ".";
#endif

    imLog("Indexing Local Resources... ");

    if (!file_exists(s_rootPath + "/sho.dni")) {
        imLog("Fatal: Cannot read sho.dni!");
        return 1;
    }
    s_vfs.addDniFile(s_rootPath + "/sho.dni");

    if (!file_exists(s_rootPath + "/mara.dni")) {
        imLog("Fatal: Cannot read mara.dni!");
        return 1;
    }
    s_vfs.addDniFile(s_rootPath + "/mara.dni");

    if (!file_exists(s_rootPath + "/pera.dni")) {
        imLog("Fatal: Cannot read pera.dni!");
        return 1;
    }
    s_vfs.addDniFile(s_rootPath + "/pera.dni");

    // Add the game dir last, so duplicates use the physical copy
    s_vfs.addPhysicalPath(s_rootPath);

    imLog("Reading Scene Database...");
    imSceneDatabase sdb;
    imStream* stream = s_vfs.open("/scn/myst.idx");
    if (stream == 0) {
        imLog("Fatal: Cannot open Scene Database!");
        return 1;
    }
    if (!sdb.readIndex(stream)) {
        delete stream;
        imLog("Fatal: Error reading Scene Database!");
        return 1;
    }
    delete stream;

    std::vector<unsigned int> roomKeys = sdb.room("open");
    std::vector<unsigned int>::iterator k;
    for (k = roomKeys.begin(); k != roomKeys.end(); ++k) {
        stream = s_vfs.openSdb(*k);
        if (stream == 0) {
            imLog("Error reading SDB %d", *k);
            continue;
        }
        imLog("Loaded SDB %d", *k);
        delete stream;
    }

    // Create a window for the game
    s_display = SDL_SetVideoMode(640, 480, 32,
                                 SDL_HWSURFACE | SDL_DOUBLEBUF | SDL_OPENGL);

    /*
    imString extList = (const char*)glGetString(GL_EXTENSIONS);
    while (!extList.isEmpty()) {
        size_t sp = extList.find(' ');
        if (sp == (size_t)-1) {
            imLog("* %s", extList.data());
            break;
        }
        imLog("* %s", extList.left(sp).data());
        extList = extList.mid(sp + 1);
    }
    */

    glCompressedTexImage2D_EXT = (PFNGLCOMPRESSEDTEXIMAGE2DARBPROC)
        SDL_GL_GetProcAddress("glCompressedTexImage2DARB");

    glShadeModel(GL_SMOOTH);
    glClearDepth(1.0f);
    glClearColor(0.0f, 0.5f, 0.5f, 1.0f);
    glViewport(0, 0, 640, 480);
    glMatrixMode(GL_PROJECTION);
    glLoadIdentity();
    gluPerspective(45.0f, 640.0f/480.0f, 0.1f, 10000.0f);
    glEnable(GL_TEXTURE_2D);
    glDisable(GL_CULL_FACE);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
    glColorMask(GL_TRUE, GL_TRUE, GL_TRUE, GL_TRUE);

    imMipmap img;
    stream = s_vfs.open("/scn/maps/CreditsScene03Top.hsm");
    if (stream != 0) {
        img.read(stream);
        delete stream;
        img.upload();
    } else {
        imLog("Error reading HSM file");
    }
    img.TEST_ExportDDS("test.dds");

    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glTranslatef(0.0f, 0.0f, -4.0f);
    img.bind();

    glBegin(GL_QUADS);
    glTexCoord2f(0.0f, 1.0f);
    glVertex3f(-1.0f, -1.0f, 0.0f);
    glTexCoord2f(0.0f, 0.0f);
    glVertex3f(-1.0f,  1.0f, 0.0f);
    glTexCoord2f(1.0f, 0.0f);
    glVertex3f( 1.0f,  1.0f, 0.0f);
    glTexCoord2f(1.0f, 1.0f);
    glVertex3f( 1.0f, -1.0f, 0.0f);
    glEnd();

    SDL_GL_SwapBuffers();
    SDL_Delay(3000);
    SDL_FreeSurface(s_display);

    SDL_Quit();
    return 0;
}

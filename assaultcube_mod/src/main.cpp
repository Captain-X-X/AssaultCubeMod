// main.cpp: initialisation & main loop
#include "cube.h"

// client
#include "engine/client/client.h"
#include "engine/client/clientgame.h"
#include "engine/client/Window.h"

// renderer
#include "engine/renderer/api/gl/VertexBuffer.h"
#include "engine/renderer/api/gl/VertexArray.h"
#include "engine/renderer/api/gl/IndexBuffer.h"
#include "engine/renderer/api/gl/Shader.h"
#include "engine/renderer/render2D/Renderer2D.h"

#include "engine/renderer/shadow.h"
#include "engine/renderer/render_text.h"
#include "engine/renderer/render_model.h"
#include "engine/renderer/render_hud.h"
#include "engine/renderer/render_particles.h"
#include "engine/renderer/rendergl.h"
#include "engine/renderer/texture.h"
#include "engine/renderer/world_ocull.h"

//
#include "engine/physics.h"
#include "engine/editing.h"
#include "engine/docs.h"
#include "engine/menus.h"
#include "engine/scoreboard.h"

VARP(compatibilitymode, 0, 1, 1); // FIXME : find a better place to put this ?

#define initlog(s) clientlogf("init: " s)
int main(int argc, char **argv)
{
    pushscontext(IEXC_CFG);
    initing = INIT_RESET;

    sethomedir("bin_unix/data");
    execfile((char *)"config/init.cfg");
    restoredinits = true;
    i18nmanager i18n("AC", path("packages/locale", true));

    bool dedicated = false;
    const char* initmap = rndmapname();
    initing = NOT_INITING;

    initlog("net");
    if (enet_initialize() < 0) {
        fatal("Unable to initialise network module");
    }

    if (!dedicated) {
        initclient();
    }

    // FIXME the server executed in this way does not catch the SIGTERM or ^C
    initserver(dedicated, argc, argv); // never returns if dedicated

    initlog("world");
    empty_world(7, true);

    initlog("window: sdl 1");
    int usedcolorbits = 0, useddepthbits = 0, usedfsaa = 0;
    // setupscreen(usedcolorbits, useddepthbits, usedfsaa);

    windowInfo_s winfo;
    winfo.bFullScreen = false;
    winfo.iWidth = scr_w;
    winfo.iHeight = scr_h;
    winfo.iColourBits = colorbits;
    winfo.iDepthBits = depthbits;
    winfo.iFSAA = fsaa;
    winfo.iSDLFlags = SDL_RESIZABLE;
    winfo.iStencilBits = stencilbits;
    winfo.iVSync = vsync;
    winfo.title = "Cube: Assault Cube";
    if (g_client.window.create(winfo) == false) {
        printf("failed to create sdl window!\n");
        return -1;
    }

    g_client.window.getWinEventHandler().registerRecver(&g_client.input);

    screen = g_client.window.getSDLSurface();

    // // new renderer testing
    const windowInfo_s& winfoRef = g_client.window.getWinInfo();
    Viewport vp;
    vp.x = 0;
    vp.y = 0;
    vp.width = winfoRef.iWidth;
    vp.height = winfoRef.iHeight;

    Camera2D camera2d;
    camera2d.setViewport(vp);
    // camera2d.setPosition(Vector3(0.0f, 0.0f, 0.0f));
    // camera2d.setRotation(Vector3(0.0f, 0.0f, 0.0f));

    Renderer2D render2d;
    render2d.initialize(camera2d);

    g_MaterialMgr.createDefault();

    initlog("video: misc");
    g_client.input.keyRepeat(false);
    SDL_ShowCursor(0);

    initlog("renderer: gl 1");
    gl_checkextensions();
    gl_init(scr_w, scr_h, usedcolorbits, useddepthbits, usedfsaa);

    notexture = noworldtexture = g_TextureMgr.load("packages/misc/notexture.jpg");
    if (!notexture) {
        fatal("could not find core textures (hint: run AssaultCube from the parent of the bin directory)");
    }

    nomodel = loadmodel("misc/gib01", -1); // FIXME: need actual placeholder model
    if (!notexture) {
        fatal("could not find core models");
    }

    initlog("console");
    per_idents = false;
    // Main font file, all other font files execute from here.
    if (!execfile("config/font.cfg")) {
        fatal("cannot find default font definitions");
    }
    // Check these 2 standard fonts have been executed.
    if (!setfont("mono")) {
        fatal("no mono font specified");
    }
    if (!setfont("default")) {
        fatal("no default font specified");
    }

    loadingscreen();

    particleinit();

    initlog("sound");
    audiomgr.initsound();

    initlog("cfg");
    extern void *scoremenu, *servmenu, *searchmenu, *serverinfomenu, *kickmenu, *banmenu, *forceteammenu, *giveadminmenu, *docmenu, *applymenu;
    scoremenu = addmenu("score", "columns", false, renderscores, NULL, false, true);
    servmenu = addmenu("server", NULL, true, refreshservers, serverskey);
    searchmenu = addmenu("search", NULL, true, refreshservers, serverskey);
    serverinfomenu = addmenu("serverinfo", NULL, true, refreshservers, serverinfokey);
    kickmenu = addmenu("kick player", NULL, true, refreshsopmenu);
    banmenu = addmenu("ban player", NULL, true, refreshsopmenu);
    forceteammenu = addmenu("force team", NULL, true, refreshsopmenu);
    giveadminmenu = addmenu("give admin", NULL, true, refreshsopmenu);
    docmenu = addmenu("reference", NULL, true, renderdocmenu);
    applymenu = addmenu("apply", "apply changes now?", true, refreshapplymenu);

    exec("config/scontext.cfg");
    exec("config/locale.cfg");
    exec("config/keymap.cfg");
    exec("config/menus.cfg");
    exec("config/scripts.cfg");
    exec("config/prefabs.cfg");
    exec("config/sounds.cfg");
    exec("config/securemaps.cfg");
    exec("config/admin.cfg");
    execfile("config/servers.cfg");
    per_idents = true;

    static char resdata[] = {112, 97, 99, 107, 97, 103, 101, 115, 47, 116, 101, 120, 116, 117, 114, 101, 115, 47, 107, 117, 114, 116, 47, 107, 108, 105, 116, 101, 50, 46, 106, 112, 103, 0};
    stream *f = opengzfile(resdata, "rb");
    if (f) {
        int n = f->getlil<int>();
        for (int i = 0; i < n; i++) {
            string s;
            f->read(s, sizeof(string));
            enet_uint32 c = f->getlil<enet_uint32>();
            setresdata(s, c);
        }
        delete f;
    }

    initing = INIT_LOAD;
    if (!execfile("config/saved.cfg")) {
        exec("config/defaults.cfg");
        firstrun = true;
    }
    if (identexists("afterinit")) {
        execute("afterinit");
    }
    if (compatibilitymode) {
        per_idents = false;
        exec("config/compatibility.cfg"); // exec after saved.cfg to get "compatibilitymode", but before user scripts..
        per_idents = true;
    }
    execfile("config/autoexec.cfg");
    execfile("config/auth.cfg");
    execute("addallfavcatmenus"); // exec here, to add all categories (including those defined in autoexec.cfg)
    initing = NOT_INITING;
    uniformtexres = !hirestextures;

    initlog("models");
    preload_playermodels();
    preload_hudguns();
    preload_entmodels();

    initlog("docs");
    per_idents = false;
    execfile("config/docs.cfg");
    per_idents = true;

    initlog("localconnect");
    extern string clientmap;

    copystring(clientmap, initmap); // ac_complex for 1.0, ac_shine for 1.1, ..

    localconnect();

    // if (initscript) {
    //     execute(initscript);
    // }

    initlog("mainloop");

    inputgrab(grabinput = true);

    inmainloop = true;
    g_TextureMgr.m_mloutput = true;
    Sprite testSprite1;
    Material* pSpriteMaterial1 = g_MaterialMgr.getMaterial("white");
    Material mat0 = *pSpriteMaterial1;
    testSprite1.create(10.0f, 10.0f, 100.0f, 100.0f, Vector4(1.0f, 0.0f, 0.0f, 1.0f), pSpriteMaterial1);
    // testSprite1.create(10.0f, 10.0f, 100.0f, 100.0f, Vector4(1.0f, 1.0f, 0.0f, 1.0f));

    Sprite testSprite2;
    testSprite2.create(20.0f, 20.0f, 80.0f, 80.0f, Vector4(0.0f, 1.0f, 0.0f, 1.0f), pSpriteMaterial1);

    Sprite testSprite3;
    testSprite3.create(30.0f, 30.0f, 60.0f, 60.0f, Vector4(0.0f, 0.0f, 1.0f, 1.0f), pSpriteMaterial1);

    while (g_client.window.shouldClose()) {
        g_client.window.update();

        static int frames = 0;
        static float fps = 10.0f;
        int millis = SDL_GetTicks() - clockrealbase;
        if (clockfix) {
            millis = int(millis * (double(clockerror) / 1000000));
        }
        millis += clockvirtbase;
        if (millis < totalmillis) {
            millis = totalmillis;
        }
        limitfps(millis, totalmillis);
        int elapsed = millis - totalmillis;
        if (multiplayer(false)) {
            curtime = elapsed;
        }
        else {
            static int timeerr = 0;
            int scaledtime = elapsed * gamespeed + timeerr;
            curtime = scaledtime / 100;
            timeerr = scaledtime % 100;
            if (nextmillis && watchingdemo) {
                curtime += nextmillis;
                nextmillis = 0;
            }
            if (paused) {
                curtime = 0;
            }
        }
        lastmillis += curtime;
        totalmillis = millis;

        if (lastmillis) {
            updateworld(curtime, lastmillis);
        }

        serverslice(0);

        if (elapsed) {
            fps = (1000.0f / elapsed + fps * 10) / 11; // avoid DIV-by-0
        }
        frames++;

        audiomgr.updateaudio();

        computeraytable(camera1->o.x, camera1->o.y, dynfov());

        if (!minimized) {
            gl_drawframe(screen->w, screen->h, fps < lowfps ? fps / lowfps : (fps > highfps ? fps / highfps : 1.0f), fps);
        }
        // if (frames > 3 && !minimized) {
        //     // if (frames > 4)
        //     //     SDL_GL_SwapBuffers();
        //     // hudonlyf("test!!!!!");
        // }

        render2d.begin();
        render2d.submitSprite(testSprite1, Transform());
        render2d.submitSprite(testSprite2, Transform());
        render2d.submitSprite(testSprite3, Transform());
        render2d.end();
        render2d.flush();
        
        g_client.window.swapBuffers();
    }
    
    // render2d.shutdown();
    g_TextureMgr.shutdown();
    g_MaterialMgr.shutdown();

    quit();
    // g_client.window.destroy();
    return EXIT_SUCCESS;
}

VAR(version, 1, AC_VERSION, 0);
VAR(protocol, 1, PROTOCOL_VERSION, 0);
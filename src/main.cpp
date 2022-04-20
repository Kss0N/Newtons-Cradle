// CradleCradle.cpp : Defines the entry point for the application.
//


#include "framework.h"
#include "CradleCradle.h"

#include "AllocatorCallback.h"
#include "ErrorCallback.h"
#include "CpuDispatcher.h"
#include "FilterShader.h"

#include "Shader.h"
#include "CommandLine.h"
#include "Cradle.h"
#include "logger.h"
#include "timeCounter.h"
#include "Camera.h"
#include <fstream>
#include <iostream>
#include <sstream>


void performTheTest();
#define MAX_LOADSTRING 100

// Global Variables:

//Windows WORMS

HINSTANCE ghInstance; // current instance   
HWND    ghMainWindow; //main Windows
HWND    ghCmdWindow; //CommandLineWindow;
HACCEL  ghActiveAccelTable; //The Currently active accelerator table


static WCHAR szTitle[MAX_LOADSTRING];                  // The title bar text
static WCHAR szWindowClass[MAX_LOADSTRING];            // the main window class name

//Main variables
constexpr auto BEGIN_WIDTH = 1597;
constexpr auto BEGIN_HEIGHT = 987;
HACCEL hMainAccel = nullptr;


//PhysX WORMS

physx::PxFoundation*        P::gpFoundation;
physx::PxPhysics*           P::gpPhysics;
physx::PxCooking*           P::gpCooking;
physx::PxScene*             P::gpScene;
physx::PxTolerancesScale    P::gToleranceScale;
physx::PxPvd*               P::gpPVD;


//PhysX constants

AllocatorCallback gAllocatorCB;
ErrorCallback   gErrorCB;
CpuDispatcher gCpuDispatcher;



LRESULT CALLBACK    mainWndProc(HWND, UINT, WPARAM, LPARAM);
INT_PTR CALLBACK    About(HWND, UINT, WPARAM, LPARAM);
void performTheTest();


/// <summary>
/// 
/// </summary>
/// <param name="hInstance"></param>
/// <param name="hPrevInstance"></param>
/// <param name="lpCmdLine"></param>
/// <param name="nCmdShow"></param>
/// <returns> status code; 0, if successful exit</returns>
int APIENTRY wWinMain(_In_ HINSTANCE hInstance,
                     _In_opt_ HINSTANCE hPrevInstance,
                     _In_ LPWSTR    lpCmdLine,
                     _In_ int       nCmdShow)
{
    UNREFERENCED_PARAMETER(hPrevInstance);
    UNREFERENCED_PARAMETER(lpCmdLine);

    ghInstance = hInstance;
    LoadStringW(hInstance, IDS_APP_TITLE, szTitle, MAX_LOADSTRING);
    LoadStringW(hInstance, IDC_CRADLECRADLE, szWindowClass, MAX_LOADSTRING);


    //register class
    WNDCLASSEX wcex = {
        .cbSize = sizeof(WNDCLASSEX),
        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC | CS_PARENTDC,
        .lpfnWndProc = mainWndProc,
        .cbClsExtra = 0,
        .cbWndExtra = 0,
        .hInstance = hInstance,
        .hIcon  = LoadIcon(hInstance, MAKEINTRESOURCE(IDI_CRADLECRADLE)),
        .hCursor = LoadCursor(nullptr, IDC_ARROW),
        .hbrBackground = (HBRUSH)(COLOR_WINDOW + 1),
        .lpszMenuName = MAKEINTRESOURCEW(IDC_CRADLECRADLE),
        .lpszClassName = szWindowClass,
        .hIconSm = LoadIcon(wcex.hInstance, MAKEINTRESOURCE(IDI_SMALL)),
    };
    RegisterClassExW(&wcex);


    //Create window
    constexpr DWORD mainWinExStyles = 0L;
    constexpr DWORD mainWinStyle = WS_OVERLAPPEDWINDOW | WS_CLIPCHILDREN;
    ghMainWindow = CreateWindowEx
    (mainWinExStyles, szWindowClass, szTitle, mainWinStyle,
        //Position
        CW_USEDEFAULT, 0, 
        //Size
        BEGIN_WIDTH, BEGIN_HEIGHT, 
        nullptr, nullptr, hInstance, nullptr);

    cmd.init();
    ghCmdWindow = cmd.hCmdWind;

    /*Now onto PhysX*/
    
    P::gpFoundation = PxCreateFoundation(PX_PHYSICS_VERSION,
        gAllocatorCB, gErrorCB); //Would be a good idea to assert, but oh well

#if defined(_DEBUG) && !defined(NOPVD)
    P::gpPVD = physx::PxCreatePvd(*P::gpFoundation);
    constexpr auto fileName = "PvDTransport";
    physx::PxPvdTransport* transport = physx::PxDefaultPvdFileTransportCreate(fileName);
    P::gpPVD->connect(*transport, physx::PxPvdInstrumentationFlag::eALL);
#else
    P::gpPVD = nullptr;
#endif // _DEBUG   
    P::gpPhysics = PxCreatePhysics(PX_PHYSICS_VERSION,
        *P::gpFoundation, P::gToleranceScale, false, P::gpPVD);
    P::gpCooking = PxCreateCooking(PX_PHYSICS_VERSION, *P::gpFoundation,
        physx::PxCookingParams(P::gToleranceScale));
    PxInitExtensions(*P::gpPhysics, nullptr);



    physx::PxSceneDesc SceneDesc(P::gToleranceScale);
    SceneDesc.gravity = physx::PxVec3(0, -9.82, 0);
    SceneDesc.cpuDispatcher = physx::PxDefaultCpuDispatcherCreate(12);
    SceneDesc.filterShader = filterShader;
    P::gpScene = P::gpPhysics->createScene(SceneDesc);

#if defined(_DEBUG) && !defined(NOPVD)
    P::gpScene->getScenePvdClient()->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONSTRAINTS, true);
    P::gpScene->getScenePvdClient()->setScenePvdFlag(physx::PxPvdSceneFlag::eTRANSMIT_CONTACTS, true);
#endif // _DEBUG

    


    tc.init();

    cradle.reset();
    cradle.init();

    performTheTest();
    MessageBox(ghMainWindow, L"Simulation Finished", L"PhysX done", MB_OK);


    ShowWindow(ghMainWindow, SW_SHOW);
    UpdateWindow(ghMainWindow);
    hMainAccel = LoadAccelerators(hInstance, MAKEINTRESOURCE(IDC_CRADLECRADLE));
    ghActiveAccelTable = hMainAccel;
    MSG msg;
    //main MSG loop
    while (1)
    {
        
        //Message polling
        while (PeekMessage(&msg, nullptr, 0, 0, PM_NOREMOVE | PM_NOYIELD) != 0)
        {

            if (!GetMessage(&msg, nullptr, 0, 0)) goto SUCCESSFUL_EXIT;
            if (!TranslateAccelerator(msg.hwnd, ghActiveAccelTable, &msg))
            {
                TranslateMessage(&msg);
                DispatchMessage(&msg);
            }
        }
        RedrawWindow(ghMainWindow, nullptr, nullptr, RDW_INVALIDATE);
    }
SUCCESSFUL_EXIT:
    logger::closeLog();
    //Deal with PhysX

    cradle.dispose();
    PxCloseExtensions();
    P::gpScene      ->release();
    P::gpCooking    ->release();
    P::gpPhysics    ->release();
#if defined(_DEBUG) && ! defined(NOPVD)
    P::gpPVD->release();
    transport->release();
#endif // _DEBUG    
    P::gpFoundation ->release();

    return (int) msg.wParam;
}



inline void getClientDimensions(HWND hWnd, LPLONG pWidth, LPLONG pHeight)
{
    RECT clientRect;
    GetClientRect(hWnd, &clientRect);
    *pWidth = clientRect.right - clientRect.left;
    *pHeight= clientRect.bottom - clientRect.top;
}





static void drawCradle(LPPAINTSTRUCT ps, Shader& defShader) 
{

    for (auto& pos : cradle.getPositions()) {
        cradle.ballMesh.draw(defShader, pos);
    }
}





static void initGL(HWND hWnd, Shader* pShader)
{
    auto hDevContxt = GetDC(hWnd);
    PIXELFORMATDESCRIPTOR pfd =
    {
        sizeof(PIXELFORMATDESCRIPTOR),
        1,
        PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,    // Flags
        PFD_TYPE_RGBA,        // The kind of framebuffer. RGBA or palette.
        32,                   // Colordepth of the framebuffer.
        0, 0, 0, 0, 0, 0,
        0,
        0,
        0,
        0, 0, 0, 0,
        24,                   // Number of bits for the depthbuffer
        8,                    // Number of bits for the stencilbuffer
        0,                    // Number of Aux buffers in the framebuffer.
        PFD_MAIN_PLANE,
        0,
        0, 0, 0
    };
    auto pixelFormat = ChoosePixelFormat(hDevContxt, &pfd);
    SetPixelFormat(hDevContxt, pixelFormat, &pfd);

    auto hGLRndContxt = wglCreateContext(hDevContxt);
    wglMakeCurrent(hDevContxt, hGLRndContxt);
    glewExperimental = GL_TRUE;
    glewInit();
    int azAttribList[] = {
        WGL_CONTEXT_MAJOR_VERSION_ARB, 4,
        WGL_CONTEXT_MINOR_VERSION_ARB, 6,
        0
    };
    hGLRndContxt = wglCreateContextAttribsARB(hDevContxt, nullptr, azAttribList);
    wglMakeCurrent(hDevContxt, hGLRndContxt);

    LONG clientWidth;
    LONG clientHeight;
    getClientDimensions(hWnd, &clientWidth, &clientHeight);

    glViewport(
        //Lower left corner
        0, 0,
        //Pixels
        clientWidth, clientHeight);

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0f);
    glClearColor(0.80f, 0.80f, 0.80f, 1.0f); //Black
    pShader->init("def.vert", "def.frag");

    cradle.ballMesh.tansferBufferToGL(NULL);

    ReleaseDC(hWnd, hDevContxt);
}



//
//  FUNCTION: WndProc(HWND, UINT, WPARAM, LPARAM)
//
//  PURPOSE: Processes messages for the main window.
//
//  WM_COMMAND  - process the application menu
//  WM_PAINT    - Paint the main window
//  WM_DESTROY  - post a quit message and return
//
//
LRESULT CALLBACK mainWndProc(HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam)
{
    static Shader defaultShader;



    switch (message)
    {
    case WM_KEYDOWN:
    {
        camera.move(wParam);
    }
    case WM_COMMAND:
        {
            int wmId = LOWORD(wParam);
            // Parse the menu selections:
            switch (wmId)
            {
            case IDM_ABOUT:
                DialogBox(ghInstance, MAKEINTRESOURCE(IDD_ABOUTBOX), hWnd, About);
                break;
            case IDM_EXIT:
                DestroyWindow(hWnd);
                break;
            default:
                return DefWindowProc(hWnd, message, wParam, lParam);
            }
        }
        break;
    case WM_PAINT:
        {
            PAINTSTRUCT ps;
            HDC hDC = BeginPaint(hWnd, &ps);
            glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
            //drawCradle(&ps, defaultShader);
            SwapBuffers(hDC);
            EndPaint(hWnd, &ps);
        }
        break;
    case WM_SIZE:
    {
        //Tell CmdWindow to Resize;
        auto newClientWidth = LOWORD(lParam);
        auto newClientHeight = HIWORD(lParam);

        RECT cmdRect;
        GetClientRect(ghCmdWindow, &cmdRect);
        auto cmdHeight = cmdRect.bottom - cmdRect.top;

        MoveWindow(ghCmdWindow, 
            //Pos
            0, newClientHeight-cmdHeight, 
            //
            newClientWidth, cmdHeight, TRUE);

        glViewport(0, 0, newClientWidth, newClientHeight);

        break;
    }
    case WM_SETFOCUS:
    {
        ghActiveAccelTable = hMainAccel;
        break;
    }
    case WM_DESTROY:
        PostQuitMessage(0);
        break;
    case WM_CREATE:
    {
        LPCREATESTRUCT createStruct = (LPCREATESTRUCT)lParam;
        initGL(hWnd, &defaultShader);
    }
    case WM_ERASEBKGND:
    {
        //Do nothing

        break;
    }
    default:
        return DefWindowProc(hWnd, message, wParam, lParam);
    }
    return 0;
}

// Message handler for about box.
INT_PTR CALLBACK About(HWND hDlg, UINT message, WPARAM wParam, LPARAM lParam)
{
    UNREFERENCED_PARAMETER(lParam);
    switch (message)
    {
    case WM_INITDIALOG:
        return (INT_PTR)TRUE;

    case WM_COMMAND:
        if (LOWORD(wParam) == IDOK || LOWORD(wParam) == IDCANCEL)
        {
            EndDialog(hDlg, LOWORD(wParam));
            return (INT_PTR)TRUE;
        }
        break;
    }
    return (INT_PTR)FALSE;
}


constexpr double radians(double degrees) { return degrees * 3.1415926535897932 / 180; }




void performTheTest()
{
    std::ofstream output;

    /*1. Radius */
    
    cradle.reset();
    output.open("data\\radius.txt", std::ios::app);
    for (float r = 0.5; r < 10; r += 0.5)
    {
        for (int i = 0; i < 5; i++)
        {
            cradle.ballRadius = r;
            cradle.dispose();    
            cradle.init();
            auto ms = cradle.simulate();
            output << "radius = " << r << ", " << "time = " << ms << "\r\n";
        }
        
    }
    output.close();

    /*2. Density*/
    
    cradle.reset();
    output.open("data\\density.txt", std::ios::app);
    for (float d = 1; d <= 10; d += 0.5)
    {
        for (int i = 0; i < 5; i++)
        {
            cradle.density = d;
            cradle.dispose();
            cradle.init();
            auto ms = cradle.simulate();
            output << "density = " << d << ", " << "time = " << ms << "\r\n";
        }

    }
    output.close();

    /*3. static friction*/
    cradle.reset();
    output.open("data\\static friction.txt", std::ios::app);
    for (float sf = .05; sf <= 1; sf += .05)
    {
        for (int i = 0; i < 5; i++)
        {
            cradle.staticFriction = sf;
            cradle.dispose();
            cradle.init();
            auto ms = cradle.simulate();
            output << "static friction = " << sf << ", " << "time = " << ms << "\r\n";
        }

    }
    output.close();

    /*4. dynamic Friction*/
    cradle.reset();
    output.open("data\\dynamic friction.txt", std::ios::app);
    for (float df = .05; df <= 1; df += .05)
    {
        for (int i = 0; i < 5; i++)
        {
            cradle.dynamicFriction = df;
            cradle.dispose();
            cradle.init();
            auto ms = cradle.simulate();
            output << "dynamic friciton = " << df << ", " << "time = " << ms << "\r\n";
        }

    }
    output.close();


    /*5. dynamic Friction*/
    cradle.reset();
    output.open("data\\restitution.txt", std::ios::app);
    for (float r = .05; r <= 1; r += .05)
    {
        for (int i = 0; i < 5; i++)
        {
            cradle.restitution = r;
            cradle.dispose();
            cradle.init();
            auto ms = cradle.simulate();
            output << "restitution = " << r << ", " << "time = " << ms << "\r\n";
        }

    }
    output.close();

    /*6. angle*/
    cradle.reset();
    output.open("data\\angle.txt", std::ios::app);
    for (float a = radians(30); a < radians(90); a += .3)
    {
        for (int i = 0; i < 5; i++)
        {
            cradle.angleBetweenBallAndJoint = a;
            cradle.dispose();
            cradle.init();
            auto ms = cradle.simulate();
            output  << "angle = " << a << ", " << "time = " << ms << "\r\n";
        }

    }
    output.close();


    /*7. height*/
    cradle.reset();
    output.open("data\\height.txt", std::ios::app);
    for (float h = 2; h <= 20; h += 1)
    {
        for (int i = 0; i < 5; i++)
        {
            cradle.jointHeight = h;
            cradle.dispose();
            cradle.init();
            auto ms = cradle.simulate();
            output << "height = " << h << ", " << "time = " << ms << "\r\n";
        }

    }
    output.close();

    /*8. balls*/
    cradle.reset();
    output.open("data\\balls.txt", std::ios::app);
    for (float balls = 2; balls <= 15; balls += 1)
    {
        for (int i = 0; i < 5; i++)
        {
            cradle.dispose();
            cradle.init(balls);
            auto ms = cradle.simulate();
            output << "balls = " << balls << ", " << "time = " << ms << "\r\n";
        }

    }
    output.close();

    /*9. gravity*/
    cradle.reset();
    output.open("data\\gravity.txt", std::ios::app);
    for (float g = 1; g <= 20; g += 1)
    {
        for (int i = 0; i < 5; i++) {
            cradle.dispose();
            P::gpScene->setGravity(PxVec3(0, -g, 0));
            cradle.init();
            auto ms = cradle.simulate();
            output << "gravity = " << g << ", " << "time = " << ms << "\r\n";
        }
    }
    output.close();

}


// MFCOpenSceneGraph.cpp : implementation of the CMFCOpenSceneGraph class
//
#include "stdafx.h"
#include "MFCOpenSceneGraph.h"
#include "osgswg/swg_repository.h"
#include "MainFrm.h"

CMFCOpenSceneGraph::CMFCOpenSceneGraph(HWND hWnd, swgRepository* osg_repo) 
    : m_hWnd(hWnd) 
    , osg_repo_(osg_repo)
{
}

CMFCOpenSceneGraph::~CMFCOpenSceneGraph()
{
    mViewer->setDone(true);
    Sleep(1000);
    mViewer->stopThreading();

    delete mViewer;
}

void CMFCOpenSceneGraph::InitOSG(std::string modelname)
{
    // Store the name of the model to load
    m_ModelName = modelname;

    // Init different parts of OSG
    InitManipulators();
    InitSceneGraph();
    InitCameraConfig();
}

void CMFCOpenSceneGraph::InitManipulators(void)
{
    // Create a trackball manipulator
    trackball = new osgGA::TrackballManipulator();

    // Create a Manipulator Switcher
    keyswitchManipulator = new osgGA::KeySwitchMatrixManipulator;

    // Add our trackball manipulator to the switcher
    keyswitchManipulator->addMatrixManipulator( '1', "Trackball", trackball.get());

    // Init the switcher to the first manipulator (in this case the only manipulator)
    keyswitchManipulator->selectMatrixManipulator(0);  // Zero based index Value
}


void CMFCOpenSceneGraph::InitSceneGraph(void)
{
    // Init the main Root Node/Group
    mRoot = new osg::MatrixTransform;

    mRoot->setMatrix(
        osg::Matrix::rotate(
            osg::DegreesToRadians( 90.0 ),
            1.0, 0.0, 0.0 ));

    // Load the Model from the model name
    mModel = osg_repo_->loadFile(m_ModelName);
    if (!mModel) return;

    // Optimize the model
    osgUtil::Optimizer optimizer;
    optimizer.optimize(mModel.get());
    optimizer.reset();

    // Add the model to the scene
    mRoot->addChild(mModel.get());
}

void CMFCOpenSceneGraph::InitCameraConfig(void)
{
    // Local Variable to hold window size data
    RECT rect;

    // Create the viewer for this window
    mViewer = new osgViewer::Viewer();

    // Add a Stats Handler to the viewer
    mViewer->addEventHandler(new osgViewer::StatsHandler);
    
    // Get the current window size
    ::GetWindowRect(m_hWnd, &rect);

    // Init the GraphicsContext Traits
    osg::ref_ptr<osg::GraphicsContext::Traits> traits = new osg::GraphicsContext::Traits;

    // Init the Windata Variable that holds the handle for the Window to display OSG in.
    osg::ref_ptr<osg::Referenced> windata = new osgViewer::GraphicsWindowWin32::WindowData(m_hWnd);

    // Setup the traits parameters
    traits->x = 0;
    traits->y = 0;
    traits->width = rect.right - rect.left;
    traits->height = rect.bottom - rect.top;
    traits->windowDecoration = false;
    traits->doubleBuffer = true;
    traits->sharedContext = 0;
    traits->setInheritedWindowPixelFormat = true;
    traits->inheritedWindowData = windata;

    // Create the Graphics Context
    osg::GraphicsContext* gc = osg::GraphicsContext::createGraphicsContext(traits.get());

    // Init a new Camera (Master for this View)
    osg::ref_ptr<osg::Camera> camera = new osg::Camera;

    // Assign Graphics Context to the Camera
    camera->setGraphicsContext(gc);

    // Set the viewport for the Camera
    camera->setViewport(new osg::Viewport(traits->x, traits->y, traits->width, traits->height));

    // Set projection matrix and camera attribtues
    camera->setClearMask(GL_DEPTH_BUFFER_BIT | GL_COLOR_BUFFER_BIT);
    camera->setClearColor(osg::Vec4f(0.2f, 0.2f, 0.4f, 1.0f));
    camera->setProjectionMatrixAsPerspective(
        30.0f, static_cast<double>(traits->width)/static_cast<double>(traits->height), 1.0, 1000.0);

    // Add the Camera to the Viewer
    //mViewer->addSlave(camera.get());
    mViewer->setCamera(camera.get());

    // Add the Camera Manipulator to the Viewer
    mViewer->setCameraManipulator(keyswitchManipulator.get());

    // Set the Scene Data
    mViewer->setSceneData(mRoot.get());

    // Realize the Viewer
    mViewer->realize();

    // Correct aspect ratio
    /*double fovy,aspectRatio,z1,z2;
    mViewer->getCamera()->getProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);
    aspectRatio=double(traits->width)/double(traits->height);
    mViewer->getCamera()->setProjectionMatrixAsPerspective(fovy,aspectRatio,z1,z2);*/
}

void CMFCOpenSceneGraph::PreFrameUpdate()
{
    // Due any preframe updates in this routine
}

void CMFCOpenSceneGraph::PostFrameUpdate()
{
    // Due any postframe updates in this routine
}

void CMFCOpenSceneGraph::Render(void* ptr)
{
    CMFCOpenSceneGraph* osg = (CMFCOpenSceneGraph*)ptr;

    osgViewer::Viewer* viewer = osg->getViewer();

    // You have two options for the main viewer loop
    //      viewer->run()   or
    //      while(!viewer->done()) { viewer->frame(); }

    //viewer->run();
    while(!viewer->done())
    {
        osg->PreFrameUpdate();
        viewer->frame();
        osg->PostFrameUpdate();
        //Sleep(10);         // Use this command if you need to allow other processes to have cpu time
    }

    // For some reason this has to be here to avoid issue: 
    // if you have multiple OSG windows up 
    // and you exit one then all stop rendering
    AfxMessageBox("Exit Rendering Thread");

    _endthread();
}

CRenderingThread::CRenderingThread( CMFCOpenSceneGraph* ptr )
:   OpenThreads::Thread(), _ptr(ptr), _done(false)
{
}

CRenderingThread::~CRenderingThread()
{
    _done = true;
    while( isRunning() )
        OpenThreads::Thread::YieldCurrentThread();
}

void CRenderingThread::run()
{
    if ( !_ptr )
    {
        _done = true;
        return;
    }

    osgViewer::Viewer* viewer = _ptr->getViewer();
    do
    {
        _ptr->PreFrameUpdate();
        viewer->frame();
        _ptr->PostFrameUpdate();
    } while ( !testCancel() && !viewer->done() && !_done );
}

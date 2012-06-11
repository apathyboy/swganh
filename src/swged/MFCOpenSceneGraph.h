
#pragma once

#ifdef WIN32
#pragma warning( push )
#pragma warning( disable : 4250 )
#endif

#include <osgViewer/Viewer>
#include <osgViewer/ViewerEventHandlers>
#include <osgViewer/api/win32/GraphicsWindowWin32>
#include <osgGA/TrackballManipulator>
#include <osgGA/KeySwitchMatrixManipulator>
#include <osgDB/DatabasePager>
#include <osgDB/Registry>
#include <osgDB/ReadFile>
#include <osgUtil/Optimizer>
#include <string>

#ifdef WIN32
#pragma warning( pop )
#endif

class swgRepository;

class CMFCOpenSceneGraph
{
public:
    CMFCOpenSceneGraph(HWND hWnd, swgRepository* osg_repo);
    ~CMFCOpenSceneGraph();

    void InitOSG(std::string filename);
    void InitManipulators(void);
    void InitSceneGraph(void);
    void InitCameraConfig(void);
    void SetupWindow(void);
    void SetupCamera(void);
    void PreFrameUpdate(void);
    void PostFrameUpdate(void);
    void Done(bool value) { mDone = value; }
    bool Done(void) { return mDone; }
    static void Render(void* ptr);

    osgViewer::Viewer* getViewer() { return mViewer; }

private:
    bool mDone;
    std::string m_ModelName;
    HWND m_hWnd;
    swgRepository* osg_repo_;
    osgViewer::Viewer* mViewer;
    osg::ref_ptr<osg::Group> mRoot;
    osg::ref_ptr<osg::Node> mModel;
    osg::ref_ptr<osgGA::TrackballManipulator> trackball;
    osg::ref_ptr<osgGA::KeySwitchMatrixManipulator> keyswitchManipulator;
};

class CRenderingThread : public OpenThreads::Thread
{
public:
    CRenderingThread( CMFCOpenSceneGraph* ptr );
    virtual ~CRenderingThread();

    virtual void run();

protected:
    CMFCOpenSceneGraph* _ptr;
    bool _done;
};
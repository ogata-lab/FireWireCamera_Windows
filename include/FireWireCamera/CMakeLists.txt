set(hdrs FireWireCamera.h imagecapture.h imagecaptureIEEE1394Cam.h ImageViewer.h TimeStampUtil_forRTC.h
    PARENT_SCOPE
    )

install(FILES ${hdrs} DESTINATION ${INC_INSTALL_DIR}/${PROJECT_NAME_LOWER}
    COMPONENT library)


hunter_config(Boost VERSION ${HUNTER_Boost_VERSION}
    CMAKE_ARGS 
        BUILD_SHARED_LIBS=ON 
        Boost_USE_STATIC_LIBS=OFF
        Boost_USE_MULTITHREADED=ON
        Boost_USE_STATIC_RUNTIME=OFF
        BOOST_ALL_DYN_LINK=ON
)

hunter_config(imgui VERSION ${HUNTER_imgui_VERSION}
    CMAKE_ARGS 
        IMGUI_BUILD_EXAMPLES=OFF
        IMGUI_ENABLE_TEST_ENGINE=OFF
        IMGUI_IMPL_OPENGL3=ON
        IMGUI_IMPL_GLFW=ON
        IMGUI_IMPL_OPENGL_LOADER=GLEW
)

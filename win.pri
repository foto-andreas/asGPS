# Windows-only block
windows {
    # SSE2, faster coder over smaller, full optimization
    QMAKE_CXXFLAGS += /arch:SSE2 /Oi /Ot
}






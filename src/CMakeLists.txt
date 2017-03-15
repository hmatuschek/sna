set(sna_SOURCES main.cc logger.cc
    sna.cc abstracttask.cc scantask.cc mainwindow.cc livepanel.cc scanpanel.cc
    aboutwidget.cc portdialog.cc qcustomplot.cc)
set(sna_MOC_HEADERS 
    sna.hh abstracttask.hh scantask.hh mainwindow.hh livepanel.hh scanpanel.hh
    aboutwidget.hh portdialog.hh qcustomplot.hh)
qt5_wrap_cpp(sna_MOC_SOURCES ${sna_MOC_HEADERS})

set(sna_HEADERS logger.hh ${sna_MOC_HEADERS} )

qt5_add_resources(sna_RCC_SOURCES ../resources.qrc)

add_executable(sna ${sna_SOURCES} ${sna_MOC_SOURCES} ${sna_RCC_SOURCES})
target_link_libraries(sna ${LIBS})


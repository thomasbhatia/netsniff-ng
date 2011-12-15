PROJECT(netsniff-ng C)

SET(VERSION "0.5.6.0")
SET(BUILD_STRING "generic")

FIND_PACKAGE(Threads)

IF (CMAKE_HAVE_PTHREAD_CREATE)
	ADD_EXECUTABLE(${PROJECT_NAME} 	../hash.c
					../dissector.c
					../dissector_eth.c
					../xmalloc.c
					../strlcpy.c
					../misc.c
					../bpf.c
					../parser.c
					../psched.c
					../pcap.c
					../tlsf.c
					../rw_pcap.c
					../sg_pcap.c
					../mmap_pcap.c
					../write_or_die.c
					../rx_ring.c
					../tx_ring.c
					../mtrand.c
					../netdev.c
					../tprintf.c
					../netsniff-ng.c)
	ADD_DEFINITIONS(-DPROGNAME_STRING="${PROJECT_NAME}"
		-DVERSION_STRING="${VERSION}"
		-DBUILD_STRING="${BUILD_STRING}")

	TARGET_LINK_LIBRARIES(netsniff-ng ${CMAKE_THREAD_LIBS_INIT})
	FILE(GLOB rules_files "../rules/*.bpf")
	INSTALL(FILES ${rules_files} DESTINATION ${CONFIG_INSTALL_PATH}/rules/)
	INSTALL(FILES ../conf/ether.conf ../conf/tcp.conf ../conf/udp.conf ../conf/oui.conf DESTINATION ${CONFIG_INSTALL_PATH})
	INSTALL(TARGETS ${PROJECT_NAME} DESTINATION ${EXECUTABLE_INSTALL_PATH})
ELSE(CMAKE_HAVE_PTHREAD_CREATE)
	MESSAGE("pthread is missing on target. Skipping ${PROJECT_NAME} build.")
ENDIF(CMAKE_HAVE_PTHREAD_CREATE)

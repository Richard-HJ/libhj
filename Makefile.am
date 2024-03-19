
AM_CPPFLAGS = -Iinclude

lib_LIBRARIES = libhj.a

libhj_a_SOURCES = \
		version.h \
		include/arch.h \
		Stats/CPUStat.c    Stats/CPUStat.h  Stats/CPU_net_maxnum.h Stats/CPU_net_names.h \
		Stats/net_snmp.c   Stats/net_snmp.h Stats/CPUStat.h  Stats/CPU_net_maxnum.h  Stats/CPU_net_names.h \
		Stats/disk_io.c    Stats/disk_io.h  Stats/CPUStat.h  Stats/CPU_net_maxnum.h \
		Stats/NIC_Stats.c  Stats/NIC_Stats.h \
		Stats/RDMA_Stats.c Stats/RDMA_Stats.h \
		Stats/Statistics.c Stats/Statistics.h \
		hist/hist.c hist/hist.h \
		InfoStore/InfoStore.c InfoStore/InfoStore.h \
		LsFit/LsFit.c LsFit/LsFit.h \
		RingBuf/RingBuf.c        RingBuf/RingBuf.h \
		RingBuf/TransientBuf.c   RingBuf/TransientBuf.h \
		socks/create_tcp_socket.c socks/soc_info.h \
		socks/create_udp_socket.c socks/soc_info.h \
		socks/create_raw_socket.c socks/soc_info.h \
		socks/sock_functions.c    socks/soc_info.h \
		socks/print_soc_opt.c     socks/soc_info.h \
		socks/prt_tcp_info.c      socks/soc_info.h \
		timers/StopWatch.c  timers/StopWatch.h Stats/Statistics.h\
		timers/RealTime.c   timers/RealTime.h  Stats/Statistics.h\
		sys_utils/CPUid.c        sys_utils/sys_utils.h \
		sys_utils/cpu_affinity.c sys_utils/sys_utils.h \
		Eth_lib/Eth_lib.c   Eth_lib/Eth_lib.h \
		user_if/menu.c      user_if/user_if.h \
		user_if/param_io.c  user_if/param_io.h \
		user_if/line_io.c   user_if/user_if.h 
 
pkginclude_HEADERS = \
				version.h \
				include/arch.h \
				Stats/CPU_net_maxnum.h \
				Stats/CPUStat.h \
				Stats/net_snmp.h \
				Stats/disk_io.h \
				Stats/NIC_Stats.h \
				Stats/RDMA_Stats.h \
				Stats/Statistics.h \
				hist/hist.h \
				InfoStore/InfoStore.h \
				LsFit/LsFit.h \
				RingBuf/RingBuf.h \
				RingBuf/TransientBuf.h \
				socks/soc_info.h \
				timers/RealTime.h timers/StopWatch.h \
				sys_utils/sys_utils.h \
				Eth_lib/Eth_lib.h \
				user_if/user_if.h \
				user_if/param_io.h
# for AMD have to have different compile flags
AMCFLAGS =
ARCH = @ARCH@
libhj_a_CFLAGS  = -Wall -O4 -D$(ARCH)

EXTRA_DIST = libhj.spec libhj.spec.in bootstrap README

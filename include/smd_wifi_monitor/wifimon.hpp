#ifndef _wifimon_hpp
#define _wifimon_hpp

#include <ros/ros.h>
#include <ros/timer.h>
#include <nodelet/nodelet.h>

#include <netlink/socket.h>
#include <linux/nl80211.h>

#include <vector>
#include <string>

namespace wifimon
{
	const char * ifmodes[NL80211_IFTYPE_MAX + 2] = {
		"unspecified",
		"IBSS",
		"managed",
		"AP",
		"AP/VLAN",
		"WDS",
		"monitor",
		"mesh point",
		"P2P-client",
		"P2P-GO",
		"P2P-device",
		"unknown"
	};

	const char * bss_stats[5] = {
		"unspecified",
		"associated",
		"authenticated",
		"joined",
		"unknown"
	};

	class WifiMon : public nodelet::Nodelet
	{
	public:
		WifiMon( );
		~WifiMon( );
		virtual void onInit( );
	private:
		struct sta_info
		{
			// From GET_STATION
			unsigned int last_seen;
			unsigned int rx_pkts;
			unsigned int tx_pkts;
			unsigned int rx_bytes;
			unsigned int tx_bytes;
			short int signal;
			float bitrate;
		};

		struct iface_info
		{
			bool valid;

			// From ioctl
			unsigned char mac_addr[8];

			// From GET_INTERFACE
			std::string ifname;
			enum nl80211_iftype iftype;
			int wiphy;

			// From GET_SCAN
			enum nl80211_bss_status bss_stat;
			unsigned int freq;
			std::string essid;

			std::map<uint64_t, struct sta_info> stations;
		};

		int get_info( signed long long int dev_id, struct iface_info *info );
		int real_get_iface_handler( struct nl_msg *msg );
		static const char * iftype_name( enum nl80211_iftype iftype );
		static const char * bss_stat_name( enum nl80211_bss_status iftype );
		static const std::string mac2str( unsigned char *arg );
		static int get_iface_handler( struct nl_msg *msg, void *arg );
		static int get_link_bss_handler( struct nl_msg *msg, void *arg );
		static int get_link_sta_handler( struct nl_msg *msg, void *arg );
		static int error_handler( struct sockaddr_nl *nla, struct nlmsgerr *err, void *arg );
		static int finish_handler( struct nl_msg *msg, void *arg );
		static int ack_handler( struct nl_msg *msg, void *arg );
		void PollCB( const ros::TimerEvent &e );

		struct nl_sock *nlh;
		int ioctl_sock;
		int nl80211_id;
		std::vector<signed long long int> dev_ids;
		std::vector<int> phy_ids;
		double poll_interval;

		ros::NodeHandle nh;
		ros::NodeHandle nh_priv;
		ros::Timer poll_timer;
		ros::Publisher wifi_info_pub;
	};
}

#endif /* _wifimon_hpp */


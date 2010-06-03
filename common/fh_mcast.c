/*
 * Copyright (C) 2008, 2009, 2010 The Collaborative Software Foundation.
 *
 * This file is part of FeedHandlers (FH).
 *
 * FH is free software: you can redistribute it and/or modify it under the terms of the
 * GNU Lesser General Public License as published by the Free Software Foundation, either version 3
 * of the License, or (at your option) any later version.
 *
 * FH is distributed in the hope that it will be useful, but WITHOUT ANY WARRANTY; without
 * even the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with FH.  If not, see <http://www.gnu.org/licenses/>.
 */

#include <stdio.h>
#include <errno.h>
#include "fh_log.h"
#include "fh_net.h"
#include "fh_mcast.h"

/*
 * fh_mcast_ttl
 *
 * Set the TTL on the multicast socket
 */
FH_STATUS fh_mcast_ttl(int s, uint8_t ttl)
{
    if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_TTL, (char*)&ttl, sizeof(ttl)) < 0) {
        FH_LOG(NET, ERR, ("NET> setsockopt IP_MULTICAST_TTL(%d) failed on socket #%d: %d",
            ttl, s, errno));
        return FH_ERROR;
    }

    return FH_OK;
}

/*
 * fh_mcast_if
 *
 * Bind the multicast socket to a given interface.
 */
FH_STATUS fh_mcast_if(int s, char *ifname)
{
    uint32_t ifaddr;

    ifaddr = fh_net_ifaddr(s, ifname);
    if (ifaddr == 0) {
        FH_LOG(NET, ERR, ("NET> Failed to get interface address (%s)", ifname));
        return FH_ERROR;
    }

    if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_IF, (char*)&ifaddr, sizeof(ifaddr)) < 0) {
        FH_LOG(NET, ERR, ("NET> setsockopt IP_MULTICAST_IF(%s/%s) failed on socket #%d: %d",
            fh_net_ntoa(ifaddr), ifname, s, errno));
        return FH_ERROR;
    }

    return FH_OK;
}

/*
 * fh_mcast_loop
 *
 * Enable/disable loopback for this multicast traffic that is generated by
 * this host but also listened to.
 */
FH_STATUS fh_mcast_loop(int s, int on)
{
    if (setsockopt(s, IPPROTO_IP, IP_MULTICAST_LOOP, (char*)&on, sizeof(on)) < 0) {
        FH_LOG(NET, ERR, ("NET> setsockopt IP_MULTICAST_LOOP(%s) failed on socket #%d: %d",
            on ? "on" : "off", s, errno));
        return FH_ERROR;
    }

    return FH_OK;
}

/*
 * fh_mcast_join
 *
 * Join a multicast group.
 */
FH_STATUS fh_mcast_join(int s, uint32_t ifaddr, uint32_t mcaddr)
{
    struct ip_mreq mreq;

    mreq.imr_multiaddr.s_addr = mcaddr;
    mreq.imr_interface.s_addr = ifaddr;

    if (setsockopt(s, IPPROTO_IP, IP_ADD_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0) {
        FH_LOG(NET, ERR, ("NET> setsockopt IP_ADD_MEMBERSHIP(%s) failed on socket #%d: %d",
            fh_net_ntoa(mcaddr), s, errno));
        return FH_ERROR;
    }

    return FH_OK;
}

/*
 * fh_mcast_leave
 *
 * Leave a multicast group.
 */
FH_STATUS fh_mcast_leave(int s, uint32_t ifaddr, uint32_t mcaddr)
{
    struct ip_mreq mreq;

    mreq.imr_multiaddr.s_addr = mcaddr;
    mreq.imr_interface.s_addr = ifaddr;

    if (setsockopt(s, IPPROTO_IP, IP_DROP_MEMBERSHIP, (char *)&mreq, sizeof(mreq)) < 0) {
        FH_LOG(NET, ERR, ("NET> setsockopt IP_DROP_MEMBERSHIP(%s) failed on socket #%d: %d",
            fh_net_ntoa(mcaddr), s, errno));
        return FH_ERROR;
    }

    return FH_OK;
}
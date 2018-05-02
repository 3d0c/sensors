/*---------------------------------------------------------------------------*/
/* RADIO                                                                     */
/*---------------------------------------------------------------------------*/

#undef NETSTACK_CONF_MAC
#define NETSTACK_CONF_MAC     		csma_driver

#undef NETSTACK_CONF_RDC
#define NETSTACK_CONF_RDC     		nullrdc_driver

#undef SKY_CONF_MAX_TX_POWER
#define SKY_CONF_MAX_TX_POWER 	31

#if CONTIKI_TARGET_ECONOTAG
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK_HW     1
#else
#undef NULLRDC_CONF_802154_AUTOACK
#define NULLRDC_CONF_802154_AUTOACK	1
#endif

/*---------------------------------------------------------------------------*/
/* RPL & Network                                                             */
/*---------------------------------------------------------------------------*/

#undef RPL_CONF_INIT_LINK_METRIC
#define RPL_CONF_INIT_LINK_METRIC			3

#define RPL_MAX_DAG_PER_INSTANCE	2
#define RPL_MAX_INSTANCES		1

/* Z1 platform has limited RAM */

#if defined CONTIKI_TARGET_Z1

#define RPL_CONF_MAX_PARENTS_PER_DAG    12
#define NEIGHBOR_CONF_MAX_NEIGHBORS     12

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     12

//Deprecated, for old DS6 Route API, use UIP_CONF_MAX_ROUTES instead
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   12

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   12

#else

#define RPL_CONF_MAX_PARENTS_PER_DAG    24
#define NEIGHBOR_CONF_MAX_NEIGHBORS     24

#undef UIP_CONF_DS6_NBR_NBU
#define UIP_CONF_DS6_NBR_NBU     24

//Deprecated, for old DS6 Route API, use UIP_CONF_MAX_ROUTES instead
#undef UIP_CONF_DS6_ROUTE_NBU
#define UIP_CONF_DS6_ROUTE_NBU   24

#undef UIP_CONF_MAX_ROUTES
#define UIP_CONF_MAX_ROUTES   24

#undef UIP_CONF_ND6_SEND_NA
#define UIP_CONF_ND6_SEND_NA   1

#endif

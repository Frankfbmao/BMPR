Module Based Placer and PR-aware Router

While traditional Field-Programmable Gate Array design flow usually employs fine-grained tile-based placement,
modular placement is increasingly required to speed up the large-scale placement and save the synthesis
time. Moreover, the commonly used modules can be pre-synthesized and stored in the library for design
reuse to significantly save the design, verification time, and development cost. Previous work mainly focuses
on modular floorplanning without module placement information. In this article, we propose a library-based
placement and routing flow that best utilizes the pre-placed and routed modules from the library to significantly
save the execution time while achieving the minimal area-delay product. The flow supports the
static and reconfigurable modules at the same time. The modular information is represented in the B*-Tree
structure, and the B*-Tree operations are amended together with Simulated Annealing to enable a fast
search of the placement space. Different width-height ratios of the modules are exploited to achieve areadelay
product optimization. Partial reconfiguration-aware routing using pin-to-wire abutment is proposed
to connect the modules after placement. Our placer can reduce the compilation time by 65% on average
with 17% area and 8.2% delay overhead compared with the fine-grained results of Versatile Place and Route
through the reuse of module information in the library for the base architecture. For other architectures,
the area increase ranges from 8.32% to 25.79%, the delay varies from −13.66% to 19.79%, and the runtime
improves by 43.31% to 77.2%.
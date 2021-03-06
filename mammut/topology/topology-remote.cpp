#ifdef MAMMUT_REMOTE
#include "../topology/topology-remote.hpp"
#include "../topology/topology-remote.pb.h"

#include "stdexcept"

namespace mammut{
namespace topology{

TopologyRemote::TopologyRemote(Communicator* const communicator):Topology(communicator){
#ifndef MAMMUT_REMOTE
    throw std::runtime_error("You need to define MAMMUT_REMOTE macro to use "
                             "remote capabilities.");
#endif
}

static inline void setUtilization(const Communicator* communicator, SetUtilization_Type type, SetUtilization_UnitType unitType, uint id){
    SetUtilization su;
    ResultVoid r;
    su.set_type(type);
    su.set_unit_type(unitType);
    su.set_id(id);
    communicator->remoteCall(su, r);
}

void TopologyRemote::maximizeUtilization() const{
    setUtilization(_communicator, SetUtilization_Type_MAXIMIZE, SetUtilization_UnitType_TOPOLOGY, 0);
}

void TopologyRemote::resetUtilization() const{
    setUtilization(_communicator, SetUtilization_Type_RESET, SetUtilization_UnitType_TOPOLOGY, 0);
}

CpuRemote::CpuRemote(Communicator* const communicator, CpuId cpuId, std::vector<PhysicalCore*> physicalCores)
    :Cpu(cpuId, physicalCores), _communicator(communicator){
    ;
}

std::string CpuRemote::getVendorId() const{
    GetCpuVendorId gcvi;
    GetCpuVendorIdRes r;
    gcvi.set_cpu_id(getCpuId());
    _communicator->remoteCall(gcvi, r);
    return r.vendor_id();
}

std::string CpuRemote::getFamily() const{
    GetCpuFamily gcf;
    GetCpuFamilyRes r;
    gcf.set_cpu_id(getCpuId());
    _communicator->remoteCall(gcf, r);
    return r.family();
}

std::string CpuRemote::getModel() const{
    GetCpuModel gcm;
    GetCpuModelRes r;
    gcm.set_cpu_id(getCpuId());
    _communicator->remoteCall(gcm, r);
    return r.model();
}

void CpuRemote::maximizeUtilization() const{
    setUtilization(_communicator, SetUtilization_Type_MAXIMIZE, SetUtilization_UnitType_CPU, getCpuId());
}

void CpuRemote::resetUtilization() const{
    setUtilization(_communicator, SetUtilization_Type_RESET, SetUtilization_UnitType_CPU, getCpuId());
}

PhysicalCoreRemote::PhysicalCoreRemote(Communicator* const communicator, CpuId cpuId, PhysicalCoreId physicalCoreId,
                                       std::vector<VirtualCore*> virtualCores)
    :PhysicalCore(cpuId, physicalCoreId, virtualCores), _communicator(communicator){
    ;
}

void PhysicalCoreRemote::maximizeUtilization() const{
    setUtilization(_communicator, SetUtilization_Type_MAXIMIZE, SetUtilization_UnitType_PHYSICAL_CORE, getPhysicalCoreId());
}

void PhysicalCoreRemote::resetUtilization() const{
    setUtilization(_communicator, SetUtilization_Type_RESET, SetUtilization_UnitType_PHYSICAL_CORE, getPhysicalCoreId());
}

VirtualCoreIdleLevelRemote::VirtualCoreIdleLevelRemote(VirtualCoreId virtualCoreId, uint levelId, Communicator* const communicator):
    VirtualCoreIdleLevel(virtualCoreId, levelId), _communicator(communicator){
    resetTime();
    resetCount();
}

std::string VirtualCoreIdleLevelRemote::getName() const{
    IdleLevelGetName ilgn;
    IdleLevelGetNameRes r;
    ilgn.set_virtual_core_id(getVirtualCoreId());
    ilgn.set_level_id(getLevelId());
    _communicator->remoteCall(ilgn, r);
    return r.name();
}

std::string VirtualCoreIdleLevelRemote::getDesc() const{
    IdleLevelGetDesc ilgd;
    IdleLevelGetDescRes r;
    ilgd.set_virtual_core_id(getVirtualCoreId());
    ilgd.set_level_id(getLevelId());
    _communicator->remoteCall(ilgd, r);
    return r.description();
}

bool VirtualCoreIdleLevelRemote::isEnableable() const{
    IdleLevelIsEnableable ilie;
    ResultBool r;
    ilie.set_virtual_core_id(getVirtualCoreId());
    ilie.set_level_id(getLevelId());
    _communicator->remoteCall(ilie, r);
    return r.result();
}

bool VirtualCoreIdleLevelRemote::isEnabled() const{
    IdleLevelIsEnabled ilie;
    ResultBool r;
    ilie.set_virtual_core_id(getVirtualCoreId());
    ilie.set_level_id(getLevelId());
    _communicator->remoteCall(ilie, r);
    return r.result();
}

void VirtualCoreIdleLevelRemote::enable() const{
    IdleLevelEnable ile;
    ResultVoid r;
    ile.set_virtual_core_id(getVirtualCoreId());
    ile.set_level_id(getLevelId());
    _communicator->remoteCall(ile, r);
}

void VirtualCoreIdleLevelRemote::disable() const{
    IdleLevelDisable ild;
    ResultVoid r;
    ild.set_virtual_core_id(getVirtualCoreId());
    ild.set_level_id(getLevelId());
    _communicator->remoteCall(ild, r);
}

uint VirtualCoreIdleLevelRemote::getExitLatency() const{
    IdleLevelGetExitLatency ilgel;
    IdleLevelGetExitLatencyRes r;
    ilgel.set_virtual_core_id(getVirtualCoreId());
    ilgel.set_level_id(getLevelId());
    _communicator->remoteCall(ilgel, r);
    return r.exit_latency();
}

uint VirtualCoreIdleLevelRemote::getConsumedPower() const{
    IdleLevelGetConsumedPower ilgcp;
    IdleLevelGetConsumedPowerRes r;
    ilgcp.set_virtual_core_id(getVirtualCoreId());
    ilgcp.set_level_id(getLevelId());
    _communicator->remoteCall(ilgcp, r);
    return r.consumed_power();
}

uint VirtualCoreIdleLevelRemote::getAbsoluteTime() const{
    IdleLevelGetAbsTime ilgat;
    IdleLevelGetTimeRes r;
    ilgat.set_virtual_core_id(getVirtualCoreId());
    ilgat.set_level_id(getLevelId());
    _communicator->remoteCall(ilgat, r);
    return r.time();
}

uint VirtualCoreIdleLevelRemote::getTime() const{
    IdleLevelGetTime ilgt;
    IdleLevelGetTimeRes r;
    ilgt.set_virtual_core_id(getVirtualCoreId());
    ilgt.set_level_id(getLevelId());
    _communicator->remoteCall(ilgt, r);
    return r.time();
}

void VirtualCoreIdleLevelRemote::resetTime() {
    IdleLevelResetTime ilrt;
    ResultVoid r;
    ilrt.set_virtual_core_id(getVirtualCoreId());
    ilrt.set_level_id(getLevelId());
    _communicator->remoteCall(ilrt, r);
}

uint VirtualCoreIdleLevelRemote::getAbsoluteCount() const{
    IdleLevelGetAbsCount ilgac;
    IdleLevelGetCountRes r;
    ilgac.set_virtual_core_id(getVirtualCoreId());
    ilgac.set_level_id(getLevelId());
    _communicator->remoteCall(ilgac, r);
    return r.count();
}

uint VirtualCoreIdleLevelRemote::getCount() const{
    IdleLevelGetCount ilgc;
    IdleLevelGetCountRes r;
    ilgc.set_virtual_core_id(getVirtualCoreId());
    ilgc.set_level_id(getLevelId());
    _communicator->remoteCall(ilgc, r);
    return r.count();
}

void VirtualCoreIdleLevelRemote::resetCount() {
    IdleLevelResetCount ilrc;
    ResultVoid r;
    ilrc.set_virtual_core_id(getVirtualCoreId());
    ilrc.set_level_id(getLevelId());
    _communicator->remoteCall(ilrc, r);
}

VirtualCoreRemote::VirtualCoreRemote(Communicator* const communicator, CpuId cpuId, PhysicalCoreId physicalCoreId,
                                     VirtualCoreId virtualCoreId)
    :VirtualCore(cpuId, physicalCoreId, virtualCoreId), _communicator(communicator){
    IdleLevelsGet ilg;
    IdleLevelsGetRes r;
    ilg.set_virtual_core_id(getVirtualCoreId());
    _communicator->remoteCall(ilg, r);
    for(int i = 0; i < r.level_id_size(); i++){
        _idleLevels.push_back(new VirtualCoreIdleLevelRemote(getVirtualCoreId(), r.level_id(i), _communicator));
    }
    resetIdleTime();
}

bool VirtualCoreRemote::hasFlag(const std::string& flagName) const{
    HasFlag hf;
    ResultBool r;
    hf.set_virtual_core_id(getVirtualCoreId());
    hf.set_flag_name(flagName);
    _communicator->remoteCall(hf, r);
    return r.result();
}

uint64_t VirtualCoreRemote::getAbsoluteTicks() const{
    GetAbsoluteTicks gat;
    ResultUint64 r;
    gat.set_virtual_core_id(getVirtualCoreId());
    _communicator->remoteCall(gat, r);
    return r.result();
}

void VirtualCoreRemote::maximizeUtilization() const{
    setUtilization(_communicator, SetUtilization_Type_MAXIMIZE, SetUtilization_UnitType_VIRTUAL_CORE, getVirtualCoreId());
}

void VirtualCoreRemote::resetUtilization() const{
    setUtilization(_communicator, SetUtilization_Type_RESET, SetUtilization_UnitType_VIRTUAL_CORE, getVirtualCoreId());
}

double VirtualCoreRemote::getIdleTime() const{
    GetIdleTime git;
    GetIdleTimeRes r;
    git.set_virtual_core_id(getVirtualCoreId());
    _communicator->remoteCall(git, r);
    return r.idle_time();
}

void VirtualCoreRemote::resetIdleTime(){
    ResetIdleTime rit;
    ResultVoid r;
    rit.set_virtual_core_id(getVirtualCoreId());
    _communicator->remoteCall(rit, r);
}

bool VirtualCoreRemote::isHotPluggable() const{
    IsHotPluggable ihp;
    ResultBool r;
    ihp.set_virtual_core_id(getVirtualCoreId());
    _communicator->remoteCall(ihp, r);
    return r.result();
}

bool VirtualCoreRemote::isHotPlugged() const{
    IsHotPlugged ihp;
    ResultBool r;
    ihp.set_virtual_core_id(getVirtualCoreId());
    _communicator->remoteCall(ihp, r);
    return r.result();
}

void VirtualCoreRemote::hotPlug() const{
    HotPlug hp;
    ResultVoid r;
    hp.set_virtual_core_id(getVirtualCoreId());
    _communicator->remoteCall(hp, r);
}

void VirtualCoreRemote::hotUnplug() const{
    HotUnplug hu;
    ResultVoid r;
    hu.set_virtual_core_id(getVirtualCoreId());
    _communicator->remoteCall(hu, r);
}

std::vector<VirtualCoreIdleLevel*> VirtualCoreRemote::getIdleLevels() const{
    return _idleLevels;
}

VirtualCoreRemote::~VirtualCoreRemote(){
    utils::deleteVectorElements<VirtualCoreIdleLevel*>(_idleLevels);
}

}
}
#endif

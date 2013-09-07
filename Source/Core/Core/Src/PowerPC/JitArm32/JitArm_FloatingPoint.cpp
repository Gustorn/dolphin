// Copyright (C) 2003 Dolphin Project.

// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, version 2.0.

// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License 2.0 for more details.

// A copy of the GPL 2.0 should have been included with the program.
// If not, see http://www.gnu.org/licenses/

// Official SVN repository and contact information can be found at
// http://code.google.com/p/dolphin-emu/

#include "Common.h"
#include "Thunk.h"

#include "../../Core.h"
#include "../PowerPC.h"
#include "../../ConfigManager.h"
#include "../../CoreTiming.h"
#include "../PPCTables.h"
#include "ArmEmitter.h"
#include "../../HW/Memmap.h"


#include "Jit.h"
#include "JitRegCache.h"
#include "JitFPRCache.h"
#include "JitAsm.h"

void JitArm::Helper_UpdateCR1(ARMReg value)
{
	// Should just update exception flags, not do any compares.
	PanicAlert("CR1");
}

void JitArm::fabsx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)

	ARMReg vB = fpr.R0(inst.FB);
	ARMReg vD = fpr.R0(inst.FD, false);

	VABS(vD, vB);

	if (inst.Rc) Helper_UpdateCR1(vD);
}

void JitArm::fnabsx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)

	ARMReg vB = fpr.R0(inst.FB);
	ARMReg vD = fpr.R0(inst.FD, false);
	ARMReg V0 = fpr.GetReg();

	// XXX: Could be done quicker
	VABS(vD, vB);
	VMOV(V0, vD);
	VSUB(vD, vD, V0);
	VSUB(vD, vD, V0);

	fpr.Unlock(V0);
	if (inst.Rc) Helper_UpdateCR1(vD);
}

void JitArm::fnegx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)

	ARMReg vB = fpr.R0(inst.FB);
	ARMReg vD = fpr.R0(inst.FD, false);

	VNEG(vD, vB);

	if (inst.Rc) Helper_UpdateCR1(vD);
}

void JitArm::faddsx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)

	ARMReg vA = fpr.R0(inst.FA);
	ARMReg vB = fpr.R0(inst.FB);
	ARMReg vD0 = fpr.R0(inst.FD, false);
	ARMReg vD1 = fpr.R1(inst.FD, false);

	VADD(vD0, vA, vB);
	VMOV(vD1, vD0);
	if (inst.Rc) Helper_UpdateCR1(vD0);
}

void JitArm::faddx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)

	ARMReg vA = fpr.R0(inst.FA);
	ARMReg vB = fpr.R0(inst.FB);
	ARMReg vD = fpr.R0(inst.FD, false);

	VADD(vD, vA, vB);
	if (inst.Rc) Helper_UpdateCR1(vD);
}

void JitArm::fsubsx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)
	
	ARMReg vA = fpr.R0(inst.FA);
	ARMReg vB = fpr.R0(inst.FB);
	ARMReg vD0 = fpr.R0(inst.FD, false);
	ARMReg vD1 = fpr.R1(inst.FD, false);

	VSUB(vD0, vA, vB);
	VMOV(vD1, vD0);
	if (inst.Rc) Helper_UpdateCR1(vD0);
}

void JitArm::fsubx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)

	ARMReg vA = fpr.R0(inst.FA);
	ARMReg vB = fpr.R0(inst.FB);
	ARMReg vD = fpr.R0(inst.FD, false);

	VSUB(vD, vA, vB);
	if (inst.Rc) Helper_UpdateCR1(vD);
}

void JitArm::fmulsx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)
	
	ARMReg vA = fpr.R0(inst.FA);
	ARMReg vC = fpr.R0(inst.FC);
	ARMReg vD0 = fpr.R0(inst.FD, false);
	ARMReg vD1 = fpr.R1(inst.FD, false);

	VMUL(vD0, vA, vC);
	VMOV(vD1, vD0);
	fpr.Flush(); // Shouldn't be needed. Missing a flush somewhere
	if (inst.Rc) Helper_UpdateCR1(vD0);
}
void JitArm::fmulx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)

	ARMReg vA = fpr.R0(inst.FA);
	ARMReg vC = fpr.R0(inst.FC);
	ARMReg vD0 = fpr.R0(inst.FD, false);

	VMUL(vD0, vA, vC);
	if (inst.Rc) Helper_UpdateCR1(vD0);
}
void JitArm::fmrx(UGeckoInstruction inst)
{
	INSTRUCTION_START
	JITDISABLE(bJITFloatingPointOff)

	ARMReg vB = fpr.R0(inst.FB);
	ARMReg vD = fpr.R0(inst.FD, false);

	VMOV(vD, vB);
	
	if (inst.Rc) Helper_UpdateCR1(vD);
}


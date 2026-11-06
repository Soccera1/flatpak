/* SPDX-License-Identifier: LGPL-2.1-or-later */

/* Representative CPU-info formats audited against the unmodified Linux 7.3
 * archive. Source paths are relative to arch/. These are format fixtures,
 * not captures from machines and not claims of native execution coverage.
 * Include every architecture directory from that release, including UML.
 *
 * Some architectures deliberately export ELF_HWCAP == 0 and no named feature
 * list. Their CPU-info scalar/boolean/register fields must remain usable.
 */
static const struct {
  /* arch names the test; source provides audit provenance; cpuinfo contains
   * only the relevant excerpt. present/absent are semicolon-separated selector
   * assertions. Omitted fields are not supplied from the real test host. */
  const char *arch;
  const char *source;
  const char *cpuinfo;
  const char *present;
  const char *absent;
} cpuinfo_arch_fixtures[] = {
  /* Scalar identity and variation bits remain usable without a named flags list. */
  { "alpha", "alpha/kernel/setup.c",
    "cpu : Alpha\ncpu model : EV67\ncpu variation : 7\n",
    "cpuinfo:cpu model=EV67;cpuinfo:cpu variation#2",
    "cpuinfo:cpu model=EV6;cpuinfo:cpu variation#3" },
  /* ARC embeds punctuation and an FPU sub-description inside arbitrary field values. */
  { "arc", "arc/kernel/setup.c",
    "ISA Extn\t: atomic ll64 unalign div_rem FPU: sp dp\nExtn [ECC]\t: IC DC MMU\n",
    "cpuinfo:ISA Extn:atomic;cpuinfo:ISA Extn:dp;cpuinfo:Extn [ECC]:MMU",
    "cpuinfo:ISA Extn:missing;cpuinfo:Extn [ECC]:DCCM" },
  /* 32-bit ARM reports named capabilities and the CPU architecture as separate fields. */
  { "arm", "arm/kernel/setup.c",
    "processor : 0\nFeatures : half thumb fastmult vfp neon vfpv3\nCPU architecture: 7\n",
    "cpuinfo:Features:neon;cpuinfo:CPU architecture=7",
    "cpuinfo:Features:sve;cpuinfo:CPU architecture=8" },
  /* AArch64 uses the same generic Features selector for extended CPU capabilities. */
  { "arm64", "arm64/kernel/cpuinfo.c",
    "processor : 0\nFeatures : fp asimd evtstrm aes sve mte\n",
    "cpuinfo:Features:sve;cpuinfo:Features:mte",
    "cpuinfo:Features:missing" },
  /* CPU function registers are numeric; a bit check must not require matching every bit. */
  { "csky", "csky/kernel/cpu-probe.c",
    "C-SKY CPU model : CK860\nhint (CPU funcs): 0x00000020\nccr  (L1C & MMU): 0x80000001\n",
    "cpuinfo:hint (CPU funcs)#5;cpuinfo:ccr  (L1C & MMU)#31",
    "cpuinfo:hint (CPU funcs)#4;cpuinfo:ccr  (L1C & MMU)#30" },
  /* This kernel format publishes identity only; do not fabricate an unreported HVX flag. */
  { "hexagon", "hexagon/kernel/setup.c",
    "processor : 0\nmodel name : Hexagon Virtual Machine\n",
    "cpuinfo:model name=Hexagon Virtual Machine",
    "cpuinfo:Features:hvx;hwcap:0" },
  /* ISA identity and virtualization/translation features can be required independently. */
  { "loongarch", "loongarch/kernel/proc.c",
    "processor : 0\nISA : loongarch64\nFeatures : cpucfg lam ual lsx lasx lvz lbt_x86\n",
    "cpuinfo:Features:lvz;cpuinfo:Features:lbt_x86;cpuinfo:ISA:loongarch64",
    "cpuinfo:Features:missing" },
  /* m68k capabilities use scalar MMU/FPU names rather than an instruction-list field. */
  { "m68k", "m68k/kernel/setup_mm.c; m68k/kernel/setup_no.c",
    "CPU: 68030\nMMU: 68030\nFPU: 68882\n",
    "cpuinfo:MMU=68030;cpuinfo:FPU=68882",
    "cpuinfo:MMU=none;cpuinfo:FPU=none" },
  /* Indented HW entries are individual boolean/version fields, with separate exception
   * tokens. */
  { "microblaze", "microblaze/kernel/cpu/mb.c",
    "CPU-Family: MicroBlaze\nHW:\n Shift: yes\n MSR: yes\n MMU: 1\n MUL: v2\n FPU: v2\n Exc: unal zero fpu\n",
    "cpuinfo:Shift=yes;cpuinfo:MSR=yes;cpuinfo:MUL=v2;cpuinfo:Exc:zero;cpuinfo:MMU#0",
    "cpuinfo:MSR=no;cpuinfo:MUL=v1;cpuinfo:MMU#1" },
  /* Instruction, ASE, and implementation-option lists each retain their own qualified
   * namespace. */
  { "mips", "mips/kernel/proc.c",
    "isa : mips32 mips32r2\nASEs implemented : dsp mt\nOptions implemented : tlb ftlb segments rixiex\n",
    "cpuinfo:isa:mips32r2;cpuinfo:ASEs implemented:dsp;cpuinfo:Options implemented:ftlb",
    "cpuinfo:isa:mips64r6;cpuinfo:Options implemented:missing" },
  /* A positive hardware boolean must not imply an unrelated FPU or disabled BMX capability. */
  { "nios2", "nios2/kernel/cpuinfo.c",
    "CPU: Nios II/f\nMMU: present\nFPU: none\nHW:\n MUL: yes\n MULX: yes\n DIV: yes\n BMX: no\n CDX: yes\n",
    "cpuinfo:MMU=present;cpuinfo:MULX=yes;cpuinfo:CDX=yes",
    "cpuinfo:BMX=yes;cpuinfo:FPU=present" },
  /* Lowercase features are ordinary qualified tokens, with no architecture dispatch. */
  { "openrisc", "openrisc/kernel/setup.c",
    "features : orbis32 orfpx32\n",
    "cpuinfo:features:orbis32;cpuinfo:features:orfpx32",
    "cpuinfo:features:orbis64" },
  /* PA-RISC capabilities mix descriptive names and a parenthesized numeric summary. */
  { "parisc", "parisc/kernel/processor.c",
    "capabilities : os32 os64 nva_supported (0x06)\n",
    "cpuinfo:capabilities:os64;cpuinfo:capabilities:nva_supported",
    "cpuinfo:capabilities:needs_equivalent_aliasing" },
  /* Descriptive CPU text and the scalar MMU mode both carry useful requirements. */
  { "powerpc", "powerpc/kernel/setup-common.c",
    "cpu : POWER9, altivec supported\nMMU : Radix\n",
    "cpuinfo:cpu:altivec;cpuinfo:MMU=Radix",
    "cpuinfo:MMU=Hash" },
  /* The system-wide isa field supports individual packed letters and extended ISA names. */
  { "riscv", "riscv/kernel/cpu.c",
    "isa : rv64imafdc_zicsr_zifencei_sstc\nhart isa : rv64imafdc_zicsr_zifencei_sstc\n",
    "cpuinfo:isa:c;cpuinfo:isa:zicsr;cpuinfo:isa:sstc",
    "cpuinfo:isa:v;cpuinfo:isa:missing" },
  /* Numeric facility identifiers are list members, not register bit positions. */
  { "s390", "s390/kernel/processor.c",
    "features : esan3 zarch stfle edat\nfacilities : 0 1 2 129\n",
    "cpuinfo:features:edat;cpuinfo:facilities:129",
    "cpuinfo:facilities:130" },
  /* SuperH names its list cpu flags; qualified matching needs no alias-table addition. */
  { "sh", "sh/kernel/cpu/proc.c",
    "cpu flags : fpu ptea llsc pteaex\ncache type : split (harvard)\n",
    "cpuinfo:cpu flags:llsc;cpuinfo:cpu flags:pteaex",
    "cpuinfo:cpu flags:dsp" },
  /* SPARC capabilities are comma-separated; cryptographic subfeatures are separate tokens. */
  { "sparc", "sparc/kernel/setup_64.c; sparc/kernel/cpu.c",
    "type : sun4v\ncpucaps : flush,stbar,v9,vis3,fjfmau,aes,montmul\n",
    "cpuinfo:cpucaps:fjfmau;cpuinfo:cpucaps:montmul;cpuinfo:type=sun4v",
    "cpuinfo:cpucaps:vis2;cpuinfo:cpucaps:missing" },
  /* User Mode Linux is included as a kernel architecture, with host-style feature fields. */
  { "um", "um/kernel/um_arch.c",
    "vendor_id : User Mode Linux\nfpu : yes\nflags : fpu cmov\n",
    "cpuinfo:flags:cmov;cpuinfo:fpu=yes",
    "cpuinfo:flags:avx;cpuinfo:fpu=no" },
  /* Virtualization and power-management requirements are independent of instruction flags. */
  { "x86", "x86/kernel/cpu/proc.c",
    "flags : fpu avx vmx constant_tsc\nvmx flags : ept vpid\npower management : ts fid vid\n",
    "cpuinfo:flags:vmx;cpuinfo:vmx flags:ept;cpuinfo:power management:fid",
    "cpuinfo:flags:svm;cpuinfo:vmx flags:missing" },
  /* Cache and debug capabilities are qualified separately from the main flags list. */
  { "xtensa", "xtensa/kernel/setup.c",
    "flags : nmi debug ocd trax density boolean loop\nicache flags : lock\ndcache flags : writeback lock\n",
    "cpuinfo:flags:trax;cpuinfo:icache flags:lock;cpuinfo:dcache flags:writeback",
    "cpuinfo:flags:missing;cpuinfo:dcache flags:missing" },
};

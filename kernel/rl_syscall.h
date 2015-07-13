#ifndef __RL_SYSCALL_H__
#define __RL_SYSCALL_H__

struct _rl_idt_t {
    u16         it_limit;
    ulong       it_base;
} __attribute__((packed));

typedef struct _rl_idt_t rl_idt_t;

struct _rl_idt_gate_t {
    u16     ig_off1;
    u16     ig_sel;
    u8      ig_nome;
    u8      ig_flags;
    u16     ig_off2;
} __attribute__((packed));

typedef struct _rl_idt_gate_t rl_idt_gate_t;

typedef struct _rl_syscall_hijack_t {
    u16     sh_syscall_num;
    void    *sh_syscall_func;
} rl_syscall_hijack_t;

extern int rl_modify_syscall_table(void);
extern void rl_restore_syscall_table(void);
extern void *rl_get_sys_call_table_addr(void);
extern void rl_setback_cr0(ulong val);
extern ulong rl_clear_cr0_save(void);
extern ulong rl_get_sys_call_table(ulong call_entry, char *exp, u32 cope);
extern int rl_sysenter_hijack(void);
extern void rl_sysenter_restore(void);

#endif

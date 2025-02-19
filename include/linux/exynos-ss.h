/*
 * Copyright (c) 2013 Samsung Electronics Co., Ltd.
 *		http://www.samsung.com
 *
 * Exynos-SnapShot debugging framework for Exynos SoC
 *
 * Author: Hosung Kim <Hosung0.kim@samsung.com>
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License version 2 as
 * published by the Free Software Foundation.
 */

#ifndef EXYNOS_SNAPSHOT_H
#define EXYNOS_SNAPSHOT_H

#ifdef CONFIG_EXYNOS_SNAPSHOT
#include <asm/ptrace.h>
#include <linux/bug.h>

/* mandatory */
extern void exynos_ss_task(int cpu, void *v_task);
extern void exynos_ss_work(void *worker, void *v_task, void *fn, int en);
extern void exynos_ss_cpuidle(char *modes, unsigned state, int diff, int en);
extern void exynos_ss_suspend(void *fn, void *dev, int en);
extern void exynos_ss_irq(int irq, void *fn, unsigned int val, int en);
extern int exynos_ss_try_enable(const char *name, unsigned long long duration);
extern int exynos_ss_set_enable(const char *name, int en);
extern int exynos_ss_get_enable(const char *name);
extern int exynos_ss_save_context(void *regs);
extern int exynos_ss_save_reg(void *regs);
extern void exynos_ss_save_system(void *unused);
extern int exynos_ss_dump_panic(char *str, size_t len);
extern int exynos_ss_prepare_panic(void);
extern int exynos_ss_post_panic(void);
extern int exynos_ss_post_reboot(char *cmd);
extern int exynos_ss_set_hardlockup(int);
extern int exynos_ss_get_hardlockup(void);
extern unsigned int exynos_ss_get_item_size(char *);
extern unsigned long exynos_ss_get_item_vaddr(char *name);
extern unsigned int exynos_ss_get_item_paddr(char *);
extern bool exynos_ss_dumper_one(void *, char *, size_t, size_t *);
extern void exynos_ss_panic_handler_safe(struct pt_regs *regs);
extern unsigned long exynos_ss_get_spare_vaddr(unsigned int offset);
extern unsigned long exynos_ss_get_spare_paddr(unsigned int offset);
extern unsigned long exynos_ss_get_last_pc(unsigned int cpu);
extern unsigned long exynos_ss_get_last_pc_paddr(void);
extern void exynos_ss_hook_hardlockup_entry(void *v_regs);
extern void exynos_ss_hook_hardlockup_exit(void);
extern void exynos_ss_dump_sfr(void);
extern int exynos_ss_hook_pmsg(char *buffer, size_t count);
extern void exynos_ss_save_log(int cpu, unsigned long where);
extern int exynos_ss_get_debug_level(void);
extern int exynos_ss_get_debug_level_reg(void);

/* option */
#ifdef CONFIG_EXYNOS_SNAPSHOT_ACPM
extern void exynos_ss_acpm(unsigned long long timestamp, const char *log, unsigned int data);
#else
#define exynos_ss_acpm(a, b, c)         do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_REGULATOR
extern void exynos_ss_regulator(unsigned long long timestamp, char* f_name, unsigned int addr, unsigned int volt, unsigned int rvolt, int en);
#else
#define exynos_ss_regulator(a, b, c, d, e)         do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_THERMAL
extern void exynos_ss_thermal(void *data, unsigned int temp, char *name, unsigned int max_cooling);
#else
#define exynos_ss_thermal(a, b, c, d)	do { } while (0)
#endif

#ifndef CONFIG_EXYNOS_SNAPSHOT_MINIMIZED_MODE
extern void exynos_ss_clockevent(unsigned long long clc, int64_t delta, void *next_event);
extern void exynos_ss_printk(const char *fmt, ...);
extern void exynos_ss_printkl(size_t msg, size_t val);
#else
#define exynos_ss_clockevent(a, b, c)	do { } while (0)
#define exynos_ss_printk(...)		do { } while (0)
#define exynos_ss_printkl(a, b)		do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_IRQ_DISABLED
extern void exynos_ss_irqs_disabled(unsigned long flags);
#else
#define exynos_ss_irqs_disabled(a)	do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_HRTIMER
extern void exynos_ss_hrtimer(void *timer, s64 *now, void *fn, int en);
#else
#define exynos_ss_hrtimer(a, b, c, d)	do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_UART
struct s3c24xx_uart_port;
extern void exynos_ss_uart(struct s3c24xx_uart_port *ourport, int flag, int en);
#else
#define exynos_ss_uart(a,b,c)		do { } while(0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_I2C
struct i2c_adapter;
struct i2c_msg;
extern void exynos_ss_i2c(struct i2c_adapter *adap, struct i2c_msg *msgs, int num, int en);
#else
#define exynos_ss_i2c(a, b, c, d)	do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_SPI
struct spi_master;
struct spi_message;
extern void exynos_ss_spi(struct spi_master *master, struct spi_message *cur_msg, int en);
#else
#define exynos_ss_spi(a, b, c)		do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_REG
extern void exynos_ss_reg(unsigned int read, size_t val, size_t reg, int en);
#else
#define exynos_ss_reg(a, b, c, d)		do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_SPINLOCK
extern void exynos_ss_spinlock(void *lock, int en);
#else
#define exynos_ss_spinlock(a, b)		do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_CLK
struct clk;
extern void exynos_ss_clk(void *clock, const char *func_name, unsigned long arg, int mode);
#else
#define exynos_ss_clk(a, b, c, d)		do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_PMU
extern void exynos_ss_pmu(int id, const char *func_name, int mode);
#else
#define exynos_ss_pmu(a, b, c)		do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_FREQ
void exynos_ss_freq(int type, unsigned long old_freq, unsigned long target_freq, int en);
#else
#define exynos_ss_freq(a, b, c, d)		do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_DM
void exynos_ss_dm(int type, unsigned long min, unsigned long max, s32 wait_t, s32 t);
#else
#define exynos_ss_dm(a, b, c, d, e)		do { } while (0)
#endif

#ifdef CONFIG_EXYNOS_SNAPSHOT_IRQ_EXIT
extern void exynos_ss_irq_exit(unsigned int irq, unsigned long long start_time);
#define exynos_ss_irq_exit_var(v)	do {	v = cpu_clock(raw_smp_processor_id());	\
					} while (0)
#else
#define exynos_ss_irq_exit(a, b)		do { } while (0)
#define exynos_ss_irq_exit_var(v)	do { v = 0; } while (0)
#endif


#if defined(CONFIG_S3C2410_WATCHDOG) && defined(CONFIG_EXYNOS_SNAPSHOT_WATCHDOG_RESET)
extern int s3c2410wdt_set_emergency_stop(int index);
extern int s3c2410wdt_set_emergency_reset(unsigned int timeout, int index);
extern int s3c2410wdt_keepalive_emergency(bool reset, int index);
extern void s3c2410wdt_reset_confirm(unsigned long mtime, int index);
#else
#define s3c2410wdt_set_emergency_stop(a) 	(-1)
#define s3c2410wdt_set_emergency_reset(a, b)	do { } while (0)
#define s3c2410wdt_keepalive_emergency(a, b)	do { } while (0)
#define s3c2410wdt_reset_confirm(a, b)		do { } while (0)
#endif

#ifdef CONFIG_SEC_DEBUG
extern void exynos_ss_get_hardlockup_info(unsigned int cpu, void *info);
extern void exynos_ss_get_softlockup_info(unsigned int cpu, void *info);
#else
#define exynos_ss_get_hardlockup_info(a, b)	do { } while (0)
#define exynos_ss_get_softlockup_info(a, b)	do { } while (0)
#endif

#else
#define exynos_ss_acpm(a, b, c)		do { } while (0)
#define exynos_ss_task(a, b)		do { } while (0)
#define exynos_ss_work(a, b, c, d)	do { } while (0)
#define exynos_ss_clockevent(a, b, c)	do { } while (0)
#define exynos_ss_cpuidle(a, b, c, d)	do { } while (0)
#define exynos_ss_suspend(a, b, c)	do { } while (0)
#define exynos_ss_regulator(a, b, c, d)	do { } while (0)
#define exynos_ss_thermal(a, b, c, d)	do { } while (0)
#define exynos_ss_irq(a, b, c, d)	do { } while (0)
#define exynos_ss_irq_exit(a, b)	do { } while (0)
#define exynos_ss_irqs_disabled(a)	do { } while (0)
#define exynos_ss_spinlock(a, b)	do { } while (0)
#define exynos_ss_clk(a, b, c, d)	do { } while (0)
#define exynos_ss_pmu(a, b, c)		do { } while (0)
#define exynos_ss_freq(a, b, c, d)	do { } while (0)
#define exynos_ss_irq_exit_var(v)	do { v = 0; } while (0)
#define exynos_ss_reg(a, b, c, d)	do { } while (0)
#define exynos_ss_hrtimer(a, b, c, d)	do { } while (0)
#define exynos_ss_i2c(a, b, c, d)	do { } while (0)
#define exynos_ss_spi(a, b, c)		do { } while (0)
#define exynos_ss_hook_pmsg(a, b)	do { } while (0)
#define exynos_ss_printk(...)		do { } while (0)
#define exynos_ss_printkl(a, b)		do { } while (0)
#define exynos_ss_save_context(a)	do { } while (0)
#define exynos_ss_try_enable(a, b)	do { } while (0)
#define exynos_ss_set_enable(a, b)	do { } while (0)
#define exynos_ss_get_enable(a)		do { } while (0)
#define exynos_ss_save_reg(a)		do { } while (0)
#define exynos_ss_save_system(a)	do { } while (0)
#define exynos_ss_dump_panic(a, b)	do { } while (0)
#define exynos_ss_dump_sfr()		do { } while (0)
#define exynos_ss_prepare_panic()	do { } while (0)
#define exynos_ss_post_panic()		do { } while (0)
#define exynos_ss_post_reboot(a)	do { } while (0)
#define exynos_ss_set_hardlockup(a)	do { } while (0)
#define exynos_ss_get_hardlockup()	do { } while (0)
#define exynos_ss_get_item_size(a)	do { } while (0)
#define exynos_ss_get_item_paddr(a)	do { } while (0)
#define exynos_ss_check_crash_key(a, b)	do { } while (0)
#define exynos_ss_dm(a, b, c, d, e)	do { } while (0)
#define exynos_ss_panic_handler_safe(a)	do { } while (0)
#define exynos_ss_get_last_pc(a)	do { } while (0)
#define exynos_ss_get_last_pc_paddr()	do { } while (0)
#define exynos_ss_hook_hardlockup_entry(a) do { } while (0)
#define exynos_ss_hook_hardlockup_exit() do { } while (0)
#define exynos_ss_get_hardlockup_info(a, b)	do { } while (0)
#define exynos_ss_get_softlockup_info(a, b)	do { } while (0)

static int exynos_ss_get_debug_level(void)
{
	return 1;
}
static int exynos_ss_get_debug_level_reg(void)
{
	return 1;
}

static inline unsigned long exynos_ss_get_item_vaddr(char *name)
{
	return 0;
}
static inline bool exynos_ss_dumper_one(void *v_dumper,
				char *line, size_t size, size_t *len)
{
	return false;
}
static inline unsigned long exynos_ss_get_spare_vaddr(unsigned int offset)
{
	return 0;
}
static inline unsigned long exynos_ss_get_spare_paddr(unsigned int offset)
{
	return 0;
}
#endif /* CONFIG_EXYNOS_SNAPSHOT */

static inline void exynos_ss_bug_func(void) {}
static inline void exynos_ss_spin_func(void) {do {} while(1);}

/**
 * esslog_flag - added log information supported.
 * @ESS_FLAG_IN: Generally, marking into the function
 * @ESS_FLAG_ON: Generally, marking the status not in, not out
 * @ESS_FLAG_OUT: Generally, marking come out the function
 * @ESS_FLAG_SOFTIRQ: Marking to pass the softirq function
 * @ESS_FLAG_SOFTIRQ_HI_TASKLET: Marking to pass the tasklet function
 * @ESS_FLAG_SOFTIRQ_TASKLET: Marking to pass the tasklet function
 */
enum esslog_flag {
	ESS_FLAG_IN = 1,
	ESS_FLAG_ON = 2,
	ESS_FLAG_OUT = 3,
	ESS_FLAG_SOFTIRQ = 10000,
	ESS_FLAG_SOFTIRQ_HI_TASKLET = 10100,
	ESS_FLAG_SOFTIRQ_TASKLET = 10200,
	ESS_FLAG_CALL_TIMER_FN = 20000
};

enum esslog_freq_flag {
	ESS_FLAG_APL = 0,
	ESS_FLAG_ATL,
	ESS_FLAG_INT,
	ESS_FLAG_MIF,
	ESS_FLAG_ISP,
	ESS_FLAG_DISP,
	ESS_FLAG_INTCAM,
	ESS_FLAG_AUD,
	ESS_FLAG_IVA,
	ESS_FLAG_SCORE,
	ESS_FLAG_FSYS0,
	ESS_FLAG_END
};

#define ESS_DEBUG_LEVEL_PREFIX  (0xDB9 << 16)
#define ESS_DEBUG_LEVEL_LOW     (0)
#define ESS_DEBUG_LEVEL_MID     (1)
#endif

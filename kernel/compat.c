/*
 *  linux/kernel/compat.c
 *
 *  Kernel compatibililty routines for e.g. 32 bit syscall support
 *  on 64 bit kernels.
 *
 *  Copyright (C) 2002-2003 Stephen Rothwell, IBM Corporation
 *
 *  This program is free software; you can redistribute it and/or modify
 *  it under the terms of the GNU General Public License version 2 as
 *  published by the Free Software Foundation.
 */

#include <linux/linkage.h>
#include <linux/compat.h>
#include <linux/errno.h>
#include <linux/time.h>
#include <linux/signal.h>
#include <linux/sched.h>	/* for MAX_SCHEDULE_TIMEOUT */
#include <linux/syscalls.h>
#include <linux/unistd.h>
#include <linux/security.h>
#include <linux/timex.h>
#include <linux/export.h>
#include <linux/migrate.h>
#include <linux/posix-timers.h>
#include <linux/times.h>
#include <linux/ptrace.h>
#include <linux/gfp.h>

#include <asm/uaccess.h>

int compat_get_timex(struct timex *txc, const struct compat_timex __user *utp)
{
	struct compat_timex tx32;

	if (copy_from_user(&tx32, utp, sizeof(struct compat_timex)))
		return -EFAULT;

	txc->modes = tx32.modes;
	txc->offset = tx32.offset;
	txc->freq = tx32.freq;
	txc->maxerror = tx32.maxerror;
	txc->esterror = tx32.esterror;
	txc->status = tx32.status;
	txc->constant = tx32.constant;
	txc->precision = tx32.precision;
	txc->tolerance = tx32.tolerance;
	txc->time.tv_sec = tx32.time.tv_sec;
	txc->time.tv_usec = tx32.time.tv_usec;
	txc->tick = tx32.tick;
	txc->ppsfreq = tx32.ppsfreq;
	txc->jitter = tx32.jitter;
	txc->shift = tx32.shift;
	txc->stabil = tx32.stabil;
	txc->jitcnt = tx32.jitcnt;
	txc->calcnt = tx32.calcnt;
	txc->errcnt = tx32.errcnt;
	txc->stbcnt = tx32.stbcnt;

	return 0;
}

int compat_put_timex(struct compat_timex __user *utp, const struct timex *txc)
{
	struct compat_timex tx32;

	memset(&tx32, 0, sizeof(struct compat_timex));
	tx32.modes = txc->modes;
	tx32.offset = txc->offset;
	tx32.freq = txc->freq;
	tx32.maxerror = txc->maxerror;
	tx32.esterror = txc->esterror;
	tx32.status = txc->status;
	tx32.constant = txc->constant;
	tx32.precision = txc->precision;
	tx32.tolerance = txc->tolerance;
	tx32.time.tv_sec = txc->time.tv_sec;
	tx32.time.tv_usec = txc->time.tv_usec;
	tx32.tick = txc->tick;
	tx32.ppsfreq = txc->ppsfreq;
	tx32.jitter = txc->jitter;
	tx32.shift = txc->shift;
	tx32.stabil = txc->stabil;
	tx32.jitcnt = txc->jitcnt;
	tx32.calcnt = txc->calcnt;
	tx32.errcnt = txc->errcnt;
	tx32.stbcnt = txc->stbcnt;
	tx32.tai = txc->tai;
	if (copy_to_user(utp, &tx32, sizeof(struct compat_timex)))
		return -EFAULT;
	return 0;
}

static int __compat_get_timeval(struct timeval *tv, const struct compat_timeval __user *ctv)
{
	return (!access_ok(VERIFY_READ, ctv, sizeof(*ctv)) ||
			__get_user(tv->tv_sec, &ctv->tv_sec) ||
			__get_user(tv->tv_usec, &ctv->tv_usec)) ? -EFAULT : 0;
}

static int __compat_put_timeval(const struct timeval *tv, struct compat_timeval __user *ctv)
{
	return (!access_ok(VERIFY_WRITE, ctv, sizeof(*ctv)) ||
			__put_user(tv->tv_sec, &ctv->tv_sec) ||
			__put_user(tv->tv_usec, &ctv->tv_usec)) ? -EFAULT : 0;
}

static int __compat_get_timespec(struct timespec *ts, const struct compat_timespec __user *cts)
{
	return (!access_ok(VERIFY_READ, cts, sizeof(*cts)) ||
			__get_user(ts->tv_sec, &cts->tv_sec) ||
			__get_user(ts->tv_nsec, &cts->tv_nsec)) ? -EFAULT : 0;
}

static int __compat_put_timespec(const struct timespec *ts, struct compat_timespec __user *cts)
{
	return (!access_ok(VERIFY_WRITE, cts, sizeof(*cts)) ||
			__put_user(ts->tv_sec, &cts->tv_sec) ||
			__put_user(ts->tv_nsec, &cts->tv_nsec)) ? -EFAULT : 0;
}

static int __compat_get_timespec64(struct timespec64 *ts64,
				   const struct compat_timespec __user *cts)
{
	struct compat_timespec ts;
	int ret;

	ret = copy_from_user(&ts, cts, sizeof(ts));
	if (ret)
		return -EFAULT;

	ts64->tv_sec = ts.tv_sec;
	ts64->tv_nsec = ts.tv_nsec;

	return 0;
}

static int __compat_put_timespec64(const struct timespec64 *ts64,
				   struct compat_timespec __user *cts)
{
	struct compat_timespec ts = {
		.tv_sec = ts64->tv_sec,
		.tv_nsec = ts64->tv_nsec
	};
	return copy_to_user(cts, &ts, sizeof(ts)) ? -EFAULT : 0;
}

int compat_get_timespec64(struct timespec64 *ts, const void __user *uts)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_from_user(ts, uts, sizeof(*ts)) ? -EFAULT : 0;
	else
		return __compat_get_timespec64(ts, uts);
}
EXPORT_SYMBOL_GPL(compat_get_timespec64);

int compat_put_timespec64(const struct timespec64 *ts, void __user *uts)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_to_user(uts, ts, sizeof(*ts)) ? -EFAULT : 0;
	else
		return __compat_put_timespec64(ts, uts);
}
EXPORT_SYMBOL_GPL(compat_put_timespec64);

int compat_get_timeval(struct timeval *tv, const void __user *utv)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_from_user(tv, utv, sizeof(*tv)) ? -EFAULT : 0;
	else
		return __compat_get_timeval(tv, utv);
}
EXPORT_SYMBOL_GPL(compat_get_timeval);

int compat_put_timeval(const struct timeval *tv, void __user *utv)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_to_user(utv, tv, sizeof(*tv)) ? -EFAULT : 0;
	else
		return __compat_put_timeval(tv, utv);
}
EXPORT_SYMBOL_GPL(compat_put_timeval);

int compat_get_timespec(struct timespec *ts, const void __user *uts)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_from_user(ts, uts, sizeof(*ts)) ? -EFAULT : 0;
	else
		return __compat_get_timespec(ts, uts);
}
EXPORT_SYMBOL_GPL(compat_get_timespec);

int compat_put_timespec(const struct timespec *ts, void __user *uts)
{
	if (COMPAT_USE_64BIT_TIME)
		return copy_to_user(uts, ts, sizeof(*ts)) ? -EFAULT : 0;
	else
		return __compat_put_timespec(ts, uts);
}
EXPORT_SYMBOL_GPL(compat_put_timespec);

int compat_convert_timespec(struct timespec __user **kts,
			    const void __user *cts)
{
	struct timespec ts;
	struct timespec __user *uts;

	if (!cts || COMPAT_USE_64BIT_TIME) {
		*kts = (struct timespec __user *)cts;
		return 0;
	}

	uts = compat_alloc_user_space(sizeof(ts));
	if (!uts)
		return -EFAULT;
	if (compat_get_timespec(&ts, cts))
		return -EFAULT;
	if (copy_to_user(uts, &ts, sizeof(ts)))
		return -EFAULT;

	*kts = uts;
	return 0;
}

static long compat_nanosleep_restart(struct restart_block *restart)
{
	struct compat_timespec __user *rmtp;
	struct timespec rmt;
	mm_segment_t oldfs;
	long ret;

	restart->nanosleep.rmtp = (struct timespec __user *) &rmt;
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	ret = hrtimer_nanosleep_restart(restart);
	set_fs(oldfs);

	if (ret == -ERESTART_RESTARTBLOCK) {
		rmtp = restart->nanosleep.compat_rmtp;

		if (rmtp && compat_put_timespec(&rmt, rmtp))
			return -EFAULT;
	}

	return ret;
}

COMPAT_SYSCALL_DEFINE2(nanosleep, struct compat_timespec __user *, rqtp,
		       struct compat_timespec __user *, rmtp)
{
	struct timespec tu, rmt;
	mm_segment_t oldfs;
	long ret;

	if (compat_get_timespec(&tu, rqtp))
		return -EFAULT;

	if (!timespec_valid(&tu))
		return -EINVAL;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	ret = hrtimer_nanosleep(&tu,
				rmtp ? (struct timespec __user *)&rmt : NULL,
				HRTIMER_MODE_REL, CLOCK_MONOTONIC);
	set_fs(oldfs);

	/*
	 * hrtimer_nanosleep() can only return 0 or
	 * -ERESTART_RESTARTBLOCK here because:
	 *
	 * - we call it with HRTIMER_MODE_REL and therefor exclude the
	 *   -ERESTARTNOHAND return path.
	 *
	 * - we supply the rmtp argument from the task stack (due to
	 *   the necessary compat conversion. So the update cannot
	 *   fail, which excludes the -EFAULT return path as well. If
	 *   it fails nevertheless we have a bigger problem and wont
	 *   reach this place anymore.
	 *
	 * - if the return value is 0, we do not have to update rmtp
	 *    because there is no remaining time.
	 *
	 * We check for -ERESTART_RESTARTBLOCK nevertheless if the
	 * core implementation decides to return random nonsense.
	 */
	if (ret == -ERESTART_RESTARTBLOCK) {
		struct restart_block *restart = &current->restart_block;

		restart->fn = compat_nanosleep_restart;
		restart->nanosleep.compat_rmtp = rmtp;

		if (rmtp && compat_put_timespec(&rmt, rmtp))
			return -EFAULT;
	}
	return ret;
}

static inline long get_compat_itimerval(struct itimerval *o,
		struct compat_itimerval __user *i)
{
	return (!access_ok(VERIFY_READ, i, sizeof(*i)) ||
		(__get_user(o->it_interval.tv_sec, &i->it_interval.tv_sec) |
		 __get_user(o->it_interval.tv_usec, &i->it_interval.tv_usec) |
		 __get_user(o->it_value.tv_sec, &i->it_value.tv_sec) |
		 __get_user(o->it_value.tv_usec, &i->it_value.tv_usec)));
}

static inline long put_compat_itimerval(struct compat_itimerval __user *o,
		struct itimerval *i)
{
	return (!access_ok(VERIFY_WRITE, o, sizeof(*o)) ||
		(__put_user(i->it_interval.tv_sec, &o->it_interval.tv_sec) |
		 __put_user(i->it_interval.tv_usec, &o->it_interval.tv_usec) |
		 __put_user(i->it_value.tv_sec, &o->it_value.tv_sec) |
		 __put_user(i->it_value.tv_usec, &o->it_value.tv_usec)));
}

COMPAT_SYSCALL_DEFINE2(getitimer, int, which,
		struct compat_itimerval __user *, it)
{
	struct itimerval kit;
	int error;

	error = do_getitimer(which, &kit);
	if (!error && put_compat_itimerval(it, &kit))
		error = -EFAULT;
	return error;
}

COMPAT_SYSCALL_DEFINE3(setitimer, int, which,
		struct compat_itimerval __user *, in,
		struct compat_itimerval __user *, out)
{
	struct itimerval kin, kout;
	int error;

	if (in) {
		if (get_compat_itimerval(&kin, in))
			return -EFAULT;
	} else
		memset(&kin, 0, sizeof(kin));

	error = do_setitimer(which, &kin, out ? &kout : NULL);
	if (error || !out)
		return error;
	if (put_compat_itimerval(out, &kout))
		return -EFAULT;
	return 0;
}

static compat_clock_t clock_t_to_compat_clock_t(clock_t x)
{
	return compat_jiffies_to_clock_t(clock_t_to_jiffies(x));
}

COMPAT_SYSCALL_DEFINE1(times, struct compat_tms __user *, tbuf)
{
	if (tbuf) {
		struct tms tms;
		struct compat_tms tmp;

		do_sys_times(&tms);
		/* Convert our struct tms to the compat version. */
		tmp.tms_utime = clock_t_to_compat_clock_t(tms.tms_utime);
		tmp.tms_stime = clock_t_to_compat_clock_t(tms.tms_stime);
		tmp.tms_cutime = clock_t_to_compat_clock_t(tms.tms_cutime);
		tmp.tms_cstime = clock_t_to_compat_clock_t(tms.tms_cstime);
		if (copy_to_user(tbuf, &tmp, sizeof(tmp)))
			return -EFAULT;
	}
	force_successful_syscall_return();
	return compat_jiffies_to_clock_t(jiffies);
}

#ifdef __ARCH_WANT_SYS_SIGPENDING

/*
 * Assumption: old_sigset_t and compat_old_sigset_t are both
 * types that can be passed to put_user()/get_user().
 */

COMPAT_SYSCALL_DEFINE1(sigpending, compat_old_sigset_t __user *, set)
{
	old_sigset_t s;
	long ret;
	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);
	ret = sys_sigpending((old_sigset_t __user *) &s);
	set_fs(old_fs);
	if (ret == 0)
		ret = put_user(s, set);
	return ret;
}

#endif

#ifdef __ARCH_WANT_SYS_SIGPROCMASK

/*
 * sys_sigprocmask SIG_SETMASK sets the first (compat) word of the
 * blocked set of signals to the supplied signal set
 */
static inline void compat_sig_setmask(sigset_t *blocked, compat_sigset_word set)
{
	memcpy(blocked->sig, &set, sizeof(set));
}

COMPAT_SYSCALL_DEFINE3(sigprocmask, int, how,
		       compat_old_sigset_t __user *, nset,
		       compat_old_sigset_t __user *, oset)
{
	old_sigset_t old_set, new_set;
	sigset_t new_blocked;

	old_set = current->blocked.sig[0];

	if (nset) {
		if (get_user(new_set, nset))
			return -EFAULT;
		new_set &= ~(sigmask(SIGKILL) | sigmask(SIGSTOP));

		new_blocked = current->blocked;

		switch (how) {
		case SIG_BLOCK:
			sigaddsetmask(&new_blocked, new_set);
			break;
		case SIG_UNBLOCK:
			sigdelsetmask(&new_blocked, new_set);
			break;
		case SIG_SETMASK:
			compat_sig_setmask(&new_blocked, new_set);
			break;
		default:
			return -EINVAL;
		}

		set_current_blocked(&new_blocked);
	}

	if (oset) {
		if (put_user(old_set, oset))
			return -EFAULT;
	}

	return 0;
}

#endif

COMPAT_SYSCALL_DEFINE2(setrlimit, unsigned int, resource,
		       struct compat_rlimit __user *, rlim)
{
	struct rlimit r;

	if (!access_ok(VERIFY_READ, rlim, sizeof(*rlim)) ||
	    __get_user(r.rlim_cur, &rlim->rlim_cur) ||
	    __get_user(r.rlim_max, &rlim->rlim_max))
		return -EFAULT;

	if (r.rlim_cur == COMPAT_RLIM_INFINITY)
		r.rlim_cur = RLIM_INFINITY;
	if (r.rlim_max == COMPAT_RLIM_INFINITY)
		r.rlim_max = RLIM_INFINITY;
	return do_prlimit(current, resource, &r, NULL);
}

#ifdef COMPAT_RLIM_OLD_INFINITY

COMPAT_SYSCALL_DEFINE2(old_getrlimit, unsigned int, resource,
		       struct compat_rlimit __user *, rlim)
{
	struct rlimit r;
	int ret;
	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);
	ret = sys_old_getrlimit(resource, (struct rlimit __user *)&r);
	set_fs(old_fs);

	if (!ret) {
		if (r.rlim_cur > COMPAT_RLIM_OLD_INFINITY)
			r.rlim_cur = COMPAT_RLIM_INFINITY;
		if (r.rlim_max > COMPAT_RLIM_OLD_INFINITY)
			r.rlim_max = COMPAT_RLIM_INFINITY;

		if (!access_ok(VERIFY_WRITE, rlim, sizeof(*rlim)) ||
		    __put_user(r.rlim_cur, &rlim->rlim_cur) ||
		    __put_user(r.rlim_max, &rlim->rlim_max))
			return -EFAULT;
	}
	return ret;
}

#endif

COMPAT_SYSCALL_DEFINE2(getrlimit, unsigned int, resource,
		       struct compat_rlimit __user *, rlim)
{
	struct rlimit r;
	int ret;

	ret = do_prlimit(current, resource, NULL, &r);
	if (!ret) {
		if (r.rlim_cur > COMPAT_RLIM_INFINITY)
			r.rlim_cur = COMPAT_RLIM_INFINITY;
		if (r.rlim_max > COMPAT_RLIM_INFINITY)
			r.rlim_max = COMPAT_RLIM_INFINITY;

		if (!access_ok(VERIFY_WRITE, rlim, sizeof(*rlim)) ||
		    __put_user(r.rlim_cur, &rlim->rlim_cur) ||
		    __put_user(r.rlim_max, &rlim->rlim_max))
			return -EFAULT;
	}
	return ret;
}

int put_compat_rusage(const struct rusage *r, struct compat_rusage __user *ru)
{
	if (!access_ok(VERIFY_WRITE, ru, sizeof(*ru)) ||
	    __put_user(r->ru_utime.tv_sec, &ru->ru_utime.tv_sec) ||
	    __put_user(r->ru_utime.tv_usec, &ru->ru_utime.tv_usec) ||
	    __put_user(r->ru_stime.tv_sec, &ru->ru_stime.tv_sec) ||
	    __put_user(r->ru_stime.tv_usec, &ru->ru_stime.tv_usec) ||
	    __put_user(r->ru_maxrss, &ru->ru_maxrss) ||
	    __put_user(r->ru_ixrss, &ru->ru_ixrss) ||
	    __put_user(r->ru_idrss, &ru->ru_idrss) ||
	    __put_user(r->ru_isrss, &ru->ru_isrss) ||
	    __put_user(r->ru_minflt, &ru->ru_minflt) ||
	    __put_user(r->ru_majflt, &ru->ru_majflt) ||
	    __put_user(r->ru_nswap, &ru->ru_nswap) ||
	    __put_user(r->ru_inblock, &ru->ru_inblock) ||
	    __put_user(r->ru_oublock, &ru->ru_oublock) ||
	    __put_user(r->ru_msgsnd, &ru->ru_msgsnd) ||
	    __put_user(r->ru_msgrcv, &ru->ru_msgrcv) ||
	    __put_user(r->ru_nsignals, &ru->ru_nsignals) ||
	    __put_user(r->ru_nvcsw, &ru->ru_nvcsw) ||
	    __put_user(r->ru_nivcsw, &ru->ru_nivcsw))
		return -EFAULT;
	return 0;
}

COMPAT_SYSCALL_DEFINE4(wait4,
	compat_pid_t, pid,
	compat_uint_t __user *, stat_addr,
	int, options,
	struct compat_rusage __user *, ru)
{
	if (!ru) {
		return sys_wait4(pid, stat_addr, options, NULL);
	} else {
		struct rusage r;
		int ret;
		unsigned int status;
		mm_segment_t old_fs = get_fs();

		set_fs (KERNEL_DS);
		ret = sys_wait4(pid,
				(stat_addr ?
				 (unsigned int __user *) &status : NULL),
				options, (struct rusage __user *) &r);
		set_fs (old_fs);

		if (ret > 0) {
			if (put_compat_rusage(&r, ru))
				return -EFAULT;
			if (stat_addr && put_user(status, stat_addr))
				return -EFAULT;
		}
		return ret;
	}
}

COMPAT_SYSCALL_DEFINE5(waitid,
		int, which, compat_pid_t, pid,
		struct compat_siginfo __user *, uinfo, int, options,
		struct compat_rusage __user *, uru)
{
	siginfo_t info;
	struct rusage ru;
	long ret;
	mm_segment_t old_fs = get_fs();

	memset(&info, 0, sizeof(info));

	set_fs(KERNEL_DS);
	ret = sys_waitid(which, pid, (siginfo_t __user *)&info, options,
			 uru ? (struct rusage __user *)&ru : NULL);
	set_fs(old_fs);

	if ((ret < 0) || (info.si_signo == 0))
		return ret;

	if (uru) {
		/* sys_waitid() overwrites everything in ru */
		if (COMPAT_USE_64BIT_TIME)
			ret = copy_to_user(uru, &ru, sizeof(ru));
		else
			ret = put_compat_rusage(&ru, uru);
		if (ret)
			return -EFAULT;
	}

	BUG_ON(info.si_code & __SI_MASK);
	info.si_code |= __SI_CHLD;
	return copy_siginfo_to_user32(uinfo, &info);
}

static int compat_get_user_cpu_mask(compat_ulong_t __user *user_mask_ptr,
				    unsigned len, struct cpumask *new_mask)
{
	unsigned long *k;

	if (len < cpumask_size())
		memset(new_mask, 0, cpumask_size());
	else if (len > cpumask_size())
		len = cpumask_size();

	k = cpumask_bits(new_mask);
	return compat_get_bitmap(k, user_mask_ptr, len * 8);
}

COMPAT_SYSCALL_DEFINE3(sched_setaffinity, compat_pid_t, pid,
		       unsigned int, len,
		       compat_ulong_t __user *, user_mask_ptr)
{
	cpumask_var_t new_mask;
	int retval;

	if (!alloc_cpumask_var(&new_mask, GFP_KERNEL))
		return -ENOMEM;

	retval = compat_get_user_cpu_mask(user_mask_ptr, len, new_mask);
	if (retval)
		goto out;

	retval = sched_setaffinity(pid, new_mask);
out:
	free_cpumask_var(new_mask);
	return retval;
}

COMPAT_SYSCALL_DEFINE3(sched_getaffinity, compat_pid_t,  pid, unsigned int, len,
		       compat_ulong_t __user *, user_mask_ptr)
{
	int ret;
	cpumask_var_t mask;

	if ((len * BITS_PER_BYTE) < nr_cpu_ids)
		return -EINVAL;
	if (len & (sizeof(compat_ulong_t)-1))
		return -EINVAL;

	if (!alloc_cpumask_var(&mask, GFP_KERNEL))
		return -ENOMEM;

	ret = sched_getaffinity(pid, mask);
	if (ret == 0) {
		size_t retlen = min_t(size_t, len, cpumask_size());

		if (compat_put_bitmap(user_mask_ptr, cpumask_bits(mask), retlen * 8))
			ret = -EFAULT;
		else
			ret = retlen;
	}
	free_cpumask_var(mask);

	return ret;
}

int get_compat_itimerspec(struct itimerspec *dst,
			  const struct compat_itimerspec __user *src)
{
	if (__compat_get_timespec(&dst->it_interval, &src->it_interval) ||
	    __compat_get_timespec(&dst->it_value, &src->it_value))
		return -EFAULT;
	return 0;
}

int put_compat_itimerspec(struct compat_itimerspec __user *dst,
			  const struct itimerspec *src)
{
	if (__compat_put_timespec(&src->it_interval, &dst->it_interval) ||
	    __compat_put_timespec(&src->it_value, &dst->it_value))
		return -EFAULT;
	return 0;
}

COMPAT_SYSCALL_DEFINE3(timer_create, clockid_t, which_clock,
		       struct compat_sigevent __user *, timer_event_spec,
		       timer_t __user *, created_timer_id)
{
	struct sigevent __user *event = NULL;

	if (timer_event_spec) {
		struct sigevent kevent;

		event = compat_alloc_user_space(sizeof(*event));
		if (get_compat_sigevent(&kevent, timer_event_spec) ||
		    copy_to_user(event, &kevent, sizeof(*event)))
			return -EFAULT;
	}

	return sys_timer_create(which_clock, event, created_timer_id);
}

COMPAT_SYSCALL_DEFINE4(timer_settime, timer_t, timer_id, int, flags,
		       struct compat_itimerspec __user *, new,
		       struct compat_itimerspec __user *, old)
{
	long err;
	mm_segment_t oldfs;
	struct itimerspec newts, oldts;

	if (!new)
		return -EINVAL;
	if (get_compat_itimerspec(&newts, new))
		return -EFAULT;
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_timer_settime(timer_id, flags,
				(struct itimerspec __user *) &newts,
				(struct itimerspec __user *) &oldts);
	set_fs(oldfs);
	if (!err && old && put_compat_itimerspec(old, &oldts))
		return -EFAULT;
	return err;
}

COMPAT_SYSCALL_DEFINE2(timer_gettime, timer_t, timer_id,
		       struct compat_itimerspec __user *, setting)
{
	long err;
	mm_segment_t oldfs;
	struct itimerspec ts;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_timer_gettime(timer_id,
				(struct itimerspec __user *) &ts);
	set_fs(oldfs);
	if (!err && put_compat_itimerspec(setting, &ts))
		return -EFAULT;
	return err;
}

COMPAT_SYSCALL_DEFINE2(clock_settime, clockid_t, which_clock,
		       struct compat_timespec __user *, tp)
{
	long err;
	mm_segment_t oldfs;
	struct timespec ts;

	if (compat_get_timespec(&ts, tp))
		return -EFAULT;
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_clock_settime(which_clock,
				(struct timespec __user *) &ts);
	set_fs(oldfs);
	return err;
}

COMPAT_SYSCALL_DEFINE2(clock_gettime, clockid_t, which_clock,
		       struct compat_timespec __user *, tp)
{
	long err;
	mm_segment_t oldfs;
	struct timespec ts;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_clock_gettime(which_clock,
				(struct timespec __user *) &ts);
	set_fs(oldfs);
	if (!err && compat_put_timespec(&ts, tp))
		return -EFAULT;
	return err;
}

COMPAT_SYSCALL_DEFINE2(clock_getres, clockid_t, which_clock,
		       struct compat_timespec __user *, tp)
{
	long err;
	mm_segment_t oldfs;
	struct timespec ts;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_clock_getres(which_clock,
			       (struct timespec __user *) &ts);
	set_fs(oldfs);
	if (!err && tp && compat_put_timespec(&ts, tp))
		return -EFAULT;
	return err;
}

static long compat_clock_nanosleep_restart(struct restart_block *restart)
{
	long err;
	mm_segment_t oldfs;
	struct timespec tu;
	struct compat_timespec __user *rmtp = restart->nanosleep.compat_rmtp;

	restart->nanosleep.rmtp = (struct timespec __user *) &tu;
	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = clock_nanosleep_restart(restart);
	set_fs(oldfs);

	if ((err == -ERESTART_RESTARTBLOCK) && rmtp &&
	    compat_put_timespec(&tu, rmtp))
		return -EFAULT;

	if (err == -ERESTART_RESTARTBLOCK) {
		restart->fn = compat_clock_nanosleep_restart;
		restart->nanosleep.compat_rmtp = rmtp;
	}
	return err;
}

COMPAT_SYSCALL_DEFINE4(clock_nanosleep, clockid_t, which_clock, int, flags,
		       struct compat_timespec __user *, rqtp,
		       struct compat_timespec __user *, rmtp)
{
	long err;
	mm_segment_t oldfs;
	struct timespec in, out;
	struct restart_block *restart;

	if (compat_get_timespec(&in, rqtp))
		return -EFAULT;

	oldfs = get_fs();
	set_fs(KERNEL_DS);
	err = sys_clock_nanosleep(which_clock, flags,
				  (struct timespec __user *) &in,
				  (struct timespec __user *) &out);
	set_fs(oldfs);

	if ((err == -ERESTART_RESTARTBLOCK) && rmtp &&
	    compat_put_timespec(&out, rmtp))
		return -EFAULT;

	if (err == -ERESTART_RESTARTBLOCK) {
		restart = &current->restart_block;
		restart->fn = compat_clock_nanosleep_restart;
		restart->nanosleep.compat_rmtp = rmtp;
	}
	return err;
}

int get_compat_itimerspec64(struct itimerspec64 *its,
			const struct compat_itimerspec __user *uits)
{

	if (__compat_get_timespec64(&its->it_interval, &uits->it_interval) ||
	    __compat_get_timespec64(&its->it_value, &uits->it_value))
		return -EFAULT;
	return 0;
}
EXPORT_SYMBOL_GPL(get_compat_itimerspec64);

int put_compat_itimerspec64(const struct itimerspec64 *its,
			struct compat_itimerspec __user *uits)
{
	if (__compat_put_timespec64(&its->it_interval, &uits->it_interval) ||
	    __compat_put_timespec64(&its->it_value, &uits->it_value))
		return -EFAULT;
	return 0;
}
EXPORT_SYMBOL_GPL(put_compat_itimerspec64);

/*
 * We currently only need the following fields from the sigevent
 * structure: sigev_value, sigev_signo, sig_notify and (sometimes
 * sigev_notify_thread_id).  The others are handled in user mode.
 * We also assume that copying sigev_value.sival_int is sufficient
 * to keep all the bits of sigev_value.sival_ptr intact.
 */
int get_compat_sigevent(struct sigevent *event,
		const struct compat_sigevent __user *u_event)
{
	memset(event, 0, sizeof(*event));
	return (!access_ok(VERIFY_READ, u_event, sizeof(*u_event)) ||
		__get_user(event->sigev_value.sival_int,
			&u_event->sigev_value.sival_int) ||
		__get_user(event->sigev_signo, &u_event->sigev_signo) ||
		__get_user(event->sigev_notify, &u_event->sigev_notify) ||
		__get_user(event->sigev_notify_thread_id,
			&u_event->sigev_notify_thread_id))
		? -EFAULT : 0;
}

long compat_get_bitmap(unsigned long *mask, const compat_ulong_t __user *umask,
		       unsigned long bitmap_size)
{
	int i, j;
	unsigned long m;
	compat_ulong_t um;
	unsigned long nr_compat_longs;

	/* align bitmap up to nearest compat_long_t boundary */
	bitmap_size = ALIGN(bitmap_size, BITS_PER_COMPAT_LONG);

	if (!access_ok(VERIFY_READ, umask, bitmap_size / 8))
		return -EFAULT;

	nr_compat_longs = BITS_TO_COMPAT_LONGS(bitmap_size);

	for (i = 0; i < BITS_TO_LONGS(bitmap_size); i++) {
		m = 0;

		for (j = 0; j < sizeof(m)/sizeof(um); j++) {
			/*
			 * We dont want to read past the end of the userspace
			 * bitmap. We must however ensure the end of the
			 * kernel bitmap is zeroed.
			 */
			if (nr_compat_longs) {
				nr_compat_longs--;
				if (__get_user(um, umask))
					return -EFAULT;
			} else {
				um = 0;
			}

			umask++;
			m |= (long)um << (j * BITS_PER_COMPAT_LONG);
		}
		*mask++ = m;
	}

	return 0;
}

long compat_put_bitmap(compat_ulong_t __user *umask, unsigned long *mask,
		       unsigned long bitmap_size)
{
	int i, j;
	unsigned long m;
	compat_ulong_t um;
	unsigned long nr_compat_longs;

	/* align bitmap up to nearest compat_long_t boundary */
	bitmap_size = ALIGN(bitmap_size, BITS_PER_COMPAT_LONG);

	if (!access_ok(VERIFY_WRITE, umask, bitmap_size / 8))
		return -EFAULT;

	nr_compat_longs = BITS_TO_COMPAT_LONGS(bitmap_size);

	for (i = 0; i < BITS_TO_LONGS(bitmap_size); i++) {
		m = *mask++;

		for (j = 0; j < sizeof(m)/sizeof(um); j++) {
			um = m;

			/*
			 * We dont want to write past the end of the userspace
			 * bitmap.
			 */
			if (nr_compat_longs) {
				nr_compat_longs--;
				if (__put_user(um, umask))
					return -EFAULT;
			}

			umask++;
			m >>= 4*sizeof(um);
			m >>= 4*sizeof(um);
		}
	}

	return 0;
}

void
sigset_from_compat(sigset_t *set, const compat_sigset_t *compat)
{
	switch (_NSIG_WORDS) {
	case 4: set->sig[3] = compat->sig[6] | (((long)compat->sig[7]) << 32 );
	case 3: set->sig[2] = compat->sig[4] | (((long)compat->sig[5]) << 32 );
	case 2: set->sig[1] = compat->sig[2] | (((long)compat->sig[3]) << 32 );
	case 1: set->sig[0] = compat->sig[0] | (((long)compat->sig[1]) << 32 );
	}
}
EXPORT_SYMBOL_GPL(sigset_from_compat);

void
sigset_to_compat(compat_sigset_t *compat, const sigset_t *set)
{
	switch (_NSIG_WORDS) {
	case 4: compat->sig[7] = (set->sig[3] >> 32); compat->sig[6] = set->sig[3];
	case 3: compat->sig[5] = (set->sig[2] >> 32); compat->sig[4] = set->sig[2];
	case 2: compat->sig[3] = (set->sig[1] >> 32); compat->sig[2] = set->sig[1];
	case 1: compat->sig[1] = (set->sig[0] >> 32); compat->sig[0] = set->sig[0];
	}
}

COMPAT_SYSCALL_DEFINE4(rt_sigtimedwait, compat_sigset_t __user *, uthese,
		struct compat_siginfo __user *, uinfo,
		struct compat_timespec __user *, uts, compat_size_t, sigsetsize)
{
	compat_sigset_t s32;
	sigset_t s;
	struct timespec t;
	siginfo_t info;
	long ret;

	if (sigsetsize != sizeof(sigset_t))
		return -EINVAL;

	if (copy_from_user(&s32, uthese, sizeof(compat_sigset_t)))
		return -EFAULT;
	sigset_from_compat(&s, &s32);

	if (uts) {
		if (compat_get_timespec(&t, uts))
			return -EFAULT;
	}

	ret = do_sigtimedwait(&s, &info, uts ? &t : NULL);

	if (ret > 0 && uinfo) {
		if (copy_siginfo_to_user32(uinfo, &info))
			ret = -EFAULT;
	}

	return ret;
}

#ifdef CONFIG_NUMA
COMPAT_SYSCALL_DEFINE6(move_pages, pid_t, pid, compat_ulong_t, nr_pages,
		       compat_uptr_t __user *, pages32,
		       const int __user *, nodes,
		       int __user *, status,
		       int, flags)
{
	const void __user * __user *pages;
	int i;

	pages = compat_alloc_user_space(nr_pages * sizeof(void *));
	for (i = 0; i < nr_pages; i++) {
		compat_uptr_t p;

		if (get_user(p, pages32 + i) ||
			put_user(compat_ptr(p), pages + i))
			return -EFAULT;
	}
	return sys_move_pages(pid, nr_pages, pages, nodes, status, flags);
}

COMPAT_SYSCALL_DEFINE4(migrate_pages, compat_pid_t, pid,
		       compat_ulong_t, maxnode,
		       const compat_ulong_t __user *, old_nodes,
		       const compat_ulong_t __user *, new_nodes)
{
	unsigned long __user *old = NULL;
	unsigned long __user *new = NULL;
	nodemask_t tmp_mask;
	unsigned long nr_bits;
	unsigned long size;

	nr_bits = min_t(unsigned long, maxnode - 1, MAX_NUMNODES);
	size = ALIGN(nr_bits, BITS_PER_LONG) / 8;
	if (old_nodes) {
		if (compat_get_bitmap(nodes_addr(tmp_mask), old_nodes, nr_bits))
			return -EFAULT;
		old = compat_alloc_user_space(new_nodes ? size * 2 : size);
		if (new_nodes)
			new = old + size / sizeof(unsigned long);
		if (copy_to_user(old, nodes_addr(tmp_mask), size))
			return -EFAULT;
	}
	if (new_nodes) {
		if (compat_get_bitmap(nodes_addr(tmp_mask), new_nodes, nr_bits))
			return -EFAULT;
		if (new == NULL)
			new = compat_alloc_user_space(size);
		if (copy_to_user(new, nodes_addr(tmp_mask), size))
			return -EFAULT;
	}
	return sys_migrate_pages(pid, nr_bits + 1, old, new);
}
#endif

COMPAT_SYSCALL_DEFINE2(sched_rr_get_interval,
		       compat_pid_t, pid,
		       struct compat_timespec __user *, interval)
{
	struct timespec t;
	int ret;
	mm_segment_t old_fs = get_fs();

	set_fs(KERNEL_DS);
	ret = sys_sched_rr_get_interval(pid, (struct timespec __user *)&t);
	set_fs(old_fs);
	if (compat_put_timespec(&t, interval))
		return -EFAULT;
	return ret;
}

/*
 * Allocate user-space memory for the duration of a single system call,
 * in order to marshall parameters inside a compat thunk.
 */
void __user *compat_alloc_user_space(unsigned long len)
{
	void __user *ptr;

	/* If len would occupy more than half of the entire compat space... */
	if (unlikely(len > (((compat_uptr_t)~0) >> 1)))
		return NULL;

	ptr = arch_compat_alloc_user_space(len);

	if (unlikely(!access_ok(VERIFY_WRITE, ptr, len)))
		return NULL;

	return ptr;
}
EXPORT_SYMBOL_GPL(compat_alloc_user_space);

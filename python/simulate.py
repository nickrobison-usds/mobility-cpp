import pandas as pd
import numpy as np
import scipy.stats as sts
# Ok, let's simulate
N_SIMULATIONS = 10000
# Bucket sizes, which are are based on the bucketed_dwell_time column
bins = np.array([0, 5,  20,  60, 240, 500])


def simulate(dwell_hist):
    dwell_hist = np.array(dwell_hist).astype(np.int32)
    # Sample from the input data to
    sampled = np.random.choice(a=(bins[:-1] + bins[1:])/2, size=N_SIMULATIONS*5, p=dwell_hist / dwell_hist.sum())
    try:
        # Create a linear space of 0-300 minutes
        space = np.linspace(0, 300)
        rkde = sts.gaussian_kde(sampled)
        # Create a custom distribution from the KDE PDF for 0-300 minutes
        pdf = rkde.pdf(space)
        custm = sts.rv_discrete(name='custm', a=space.min(), b=space.max(), values=(space, pdf / pdf.sum()))
        # Sample dwell times from custom distribution
        sampled = np.array([ custm.rvs() for i in range(N_SIMULATIONS)])
        #         # Return the average dwell time
        return sampled.mean()
    except Exception as e:
        print(e)
        return 0


def prnt(x):
    return np.array(x).astype(np.int32).sum()


def compute(df):
    print("Running simulation")
    df.head()
    df['sampled'] = df.apply(lambda x: simulate(x.dwell_array), 1)
    df['contact_sim'] = df.raw_visit_counts * df.sampled
    df['contact_median'] = df.raw_visit_counts * df.median_dwell
    df.head()

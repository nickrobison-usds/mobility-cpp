def compute(df):
    age = df['age'].mean()
    df['age_diff'] = df.age - age
    return df

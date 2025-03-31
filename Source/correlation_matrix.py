import pandas as pd

if __name__ == "__main__":
    df = pd.read_csv("output_radiomics_clean.csv")
    df.corr().to_csv("cm.csv", index=True)

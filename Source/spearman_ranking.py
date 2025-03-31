import pandas as pd
from scipy.stats import spearmanr

if __name__ == "__main__":

    data = pd.read_csv("output_radiomics_clean_with_labels.csv")

    # Isolate feature columns and the label 'Progression'
    label_column = "Progression"
    feature_columns = [
        col
        for col in data.columns
        if col not in ["ID", "UID", "FileName", "ImageName", label_column]
    ]

    # Initialize a DataFrame to store Spearman correlations and p-values
    spearman_results = pd.DataFrame(columns=["Feature", "Spearman", "P-value"])

    # Calculate Spearman correlation and p-value for each feature
    for feature in feature_columns:
        spearman_corr, p_value = spearmanr(data[feature], data[label_column])
        spearman_results = spearman_results.append(
            {"Feature": feature, "Spearman": spearman_corr, "P-value": p_value},
            ignore_index=True,
        )

    # Sort results by absolute Spearman correlation (highest rank first)
    spearman_results["Rank"] = spearman_results["Spearman"].abs().rank(ascending=False)
    spearman_results = spearman_results.sort_values(by="Rank")

    print(spearman_results)

    spearman_results.to_csv(
        "/media/dhaberl/T7/PhD_Projects/Laszlo_OCTXenographProject/spearman_ranking.csv",
        index=False,
    )

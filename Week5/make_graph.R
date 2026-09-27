# Read the raw CSV data
data <- read.csv("lab5a.csv", strip.white = TRUE)

# Calculate required statistics for each worker count
summary_table <- aggregate(
    elapsed ~ workers,
    data = data,
    FUN = function(x) {
        c(
            mean = mean(x),
            minimum = min(x),
            maximum = max(x),
            range = max(x) - min(x)
        )
    }
)

# Convert the nested results into regular columns
summary_table <- data.frame(
    workers = summary_table$workers,
    mean_time = summary_table$elapsed[, "mean"],
    minimum = summary_table$elapsed[, "minimum"],
    maximum = summary_table$elapsed[, "maximum"],
    range = summary_table$elapsed[, "range"]
)

# Calculate speedup and efficiency
baseline <- summary_table$mean_time[summary_table$workers == 1]

summary_table$speedup <- baseline / summary_table$mean_time
summary_table$efficiency <- summary_table$speedup / summary_table$workers

# Display the results
print(summary_table)

# Save the graph using the required filename
pdf("lab5a_table_or_graph.pdf", width = 7, height = 5)

plot(
    summary_table$workers,
    summary_table$mean_time,
    type = "b",
    pch = 19,
    lwd = 2,
    xaxt = "n",
    xlab = "Worker Count",
    ylab = "Mean Elapsed Time (seconds)",
    main = "Mean Elapsed Time by Worker Count"
)

axis(
    1,
    at = summary_table$workers,
    labels = summary_table$workers
)

grid()
dev.off()

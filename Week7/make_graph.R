# Read the raw CSV data
data <- read.csv("memory_trials.csv", strip.white = TRUE)

# Calculate allocation-time statistics
allocation_summary <- aggregate(
    allocation_seconds ~ megabytes,
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

allocation_summary <- data.frame(
    megabytes = allocation_summary$megabytes,
    allocation_mean = allocation_summary$allocation_seconds[, "mean"],
    allocation_minimum = allocation_summary$allocation_seconds[, "minimum"],
    allocation_maximum = allocation_summary$allocation_seconds[, "maximum"],
    allocation_range = allocation_summary$allocation_seconds[, "range"]
)

# Calculate first-touch-time statistics
touch_summary <- aggregate(
    touch_seconds ~ megabytes,
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

touch_summary <- data.frame(
    megabytes = touch_summary$megabytes,
    touch_mean = touch_summary$touch_seconds[, "mean"],
    touch_minimum = touch_summary$touch_seconds[, "minimum"],
    touch_maximum = touch_summary$touch_seconds[, "maximum"],
    touch_range = touch_summary$touch_seconds[, "range"]
)

# Combine both summary tables
summary_table <- merge(
    allocation_summary,
    touch_summary,
    by = "megabytes"
)

# Display the results
print(summary_table)

# Save the graph
pdf("graph.pdf", width = 7, height = 5)

plot(
    summary_table$megabytes,
    summary_table$allocation_mean,
    type = "b",
    pch = 19,
    lwd = 2,
    col = "blue",
    log = "y",
    xaxt = "n",
    xlab = "Allocation Size (MB)",
    ylab = "Mean Time (seconds, logarithmic scale)",
    main = "Allocation and First-Touch Time by Allocation Size"
)

lines(
    summary_table$megabytes,
    summary_table$touch_mean,
    type = "b",
    pch = 17,
    lwd = 2,
    col = "red"
)

axis(
    1,
    at = summary_table$megabytes,
    labels = summary_table$megabytes
)

grid()
dev.off()

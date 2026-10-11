# Read timing data
data <- read.csv("timing.csv", strip.white = TRUE)

dir.create("graphs", showWarnings = FALSE)

common_order <- c(
    "unrestricted",
    "one_cpu",
    "two_cpu",
    "oversubscribed"
)

common <- subset(data, configuration %in% common_order)
common$configuration <- factor(
    common$configuration,
    levels = common_order
)

# -----------------------------
# Graph 1: Common configurations
# -----------------------------

common_summary <- aggregate(
    elapsed ~ configuration,
    data = common,
    FUN = mean
)

png("graphs/graph1.png", width = 900, height = 600)

barplot(
    common_summary$elapsed,
    names.arg = common_summary$configuration,
    col = "steelblue",
    ylab = "Mean Elapsed Time (seconds)",
    xlab = "Configuration",
    main = "Mean Elapsed Time by Configuration"
)

grid()

dev.off()

# -----------------------------
# Graph 2: Trial-to-trial variation
# -----------------------------

png("graphs/graph2.png", width = 900, height = 600)

boxplot(
    elapsed ~ configuration,
    data = common,
    col = "lightgreen",
    xlab = "Configuration",
    ylab = "Elapsed Time (seconds)",
    main = "Trial-to-Trial Timing Variation",
    outline = TRUE
)

grid()

dev.off()

# -----------------------------
# Graph 3: Thread-count extension
# -----------------------------

extension <- subset(
    data,
    grepl("^extension_threads_", configuration)
)

extension_summary <- aggregate(
    elapsed ~ threads,
    data = extension,
    FUN = mean
)

extension_summary <- extension_summary[
    order(extension_summary$threads),
]

print(common_summary)
print(extension_summary)

png("graphs/graph3.png", width = 900, height = 600)

plot(
    extension_summary$threads,
    extension_summary$elapsed,
    type = "b",
    pch = 19,
    lwd = 2,
    col = "firebrick",
    xlab = "Thread Count",
    ylab = "Mean Elapsed Time (seconds)",
    main = "Mean Elapsed Time by Thread Count",
    xaxt = "n"
)

axis(
    1,
    at = extension_summary$threads,
    labels = extension_summary$threads
)

grid()

dev.off()

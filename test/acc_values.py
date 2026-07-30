file = open("acc_values.txt", "r")
lines = file.read().split("\n")
file.close()

sum = 0.0
count = 0

for line in lines:
    if "Raw Accel G: " in line:
        value = float(line.split("Raw Accel G: ")[1].split(" ")[0])
        print(value)
        sum += value
        count += 1

print(sum/count)
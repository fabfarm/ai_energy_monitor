import time
import csv

with open("appliances.csv", "w+") as f:
    csv_writer = csv.writer(f, delimiter=',')
    while True:
        name = input()
        csv_writer.writerow([int(time.time()), name])

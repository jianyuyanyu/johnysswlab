python3 ../scripts/stat.py -n 10 -c "likwid-perfctr -g MEM -C 0 -m ./data_lookup -a 10000000 -v 1"
python3 ../scripts/stat.py -n 10 -c "likwid-perfctr -g MEM -C 0 -m ./data_lookup -a 10000000 -v 5"
python3 ../scripts/stat.py -n 10 -c "likwid-perfctr -g MEM -C 0 -m ./data_lookup -a 10000000 -v 20"
python3 ../scripts/stat.py -n 10 -c "likwid-perfctr -g MEM -C 0 -m ./data_lookup -a 10000000 -v 100"
python3 ../scripts/stat.py -n 10 -c "likwid-perfctr -g MEM -C 0 -m ./data_lookup -a 10000000 -v 500"
python3 ../scripts/stat.py -n 10 -c "likwid-perfctr -g MEM -C 0 -m ./data_lookup -a 10000000 -v 2000"
python3 ../scripts/stat.py -n 10 -c "likwid-perfctr -g MEM -C 0 -m ./data_lookup -a 10000000 -v 10000"

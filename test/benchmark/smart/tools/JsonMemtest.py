import json
import sys

def remove_performance(filename):
    try:
        # Open the JSON file for reading
        with open(filename, 'r') as file:
            # Load JSON content
            data = json.load(file)
            
            # Remove everything except memtest
            all = []
            for item in data['benchmarks']:
                cur = {key: value for key, value in item.items() if
                    key != 'real_time' or
                    key != 'cpu_time' or
                    key != 'time_unit'
                }
                if len(cur) > 0:
                    all.append(cur)
            data['benchmarks'] = all

            
        # Open the same file for writing (this will truncate the file)
        with open(filename, 'w') as file:
            # Write the modified JSON content back to the file
            json.dump(data, file, indent=4)
            
        print(f"Removed performance data from '{filename}'.")
    except FileNotFoundError:
        print(f"File '{filename}' not found.")
    except json.JSONDecodeError:
        print(f"File '{filename}' is not a valid JSON file.")
    except Exception as e:
        print(f"An error occurred: {str(e)}")

if __name__ == "__main__":
    if len(sys.argv) != 2:
        print("Usage: python3 removeAggr.py <filename>")
    else:
        filename = sys.argv[1]
        remove_performance(filename)

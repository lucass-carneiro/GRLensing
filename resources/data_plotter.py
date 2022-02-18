"""GRLensing data plotter.

Usage:
  data_plotter.py trajectory <trajectory_config_file> <trajectory_output_file> [--font_size=<size>] [--delta=<value>] [--plot_radius=<radius>]
  data_plotter.py energy (local|global) <trajectory_output_file> [--font_size=<size>]
  data_plotter.py penrose <penrose_config_file> <trajectory_1> <trajectory_2> <trajectory_3> [--font_size=<size>] [--color_1=<color1>] [--color_2=<color2>] [--color_3=<color3>]
  data_plotter.py (-h | --help)
  data_plotter.py --version

Options:
  -h --help               Show this screen.
  --version               Show version.
  --font_size=<size>      The size of the font in the plots [default: 20].
  --delta=<value>         The grid spacing used in internal grid generation [default: 0.1].
  --plot_radius=<radius>  The radius of the background.
  --color_1=<color1>      The color of the first trajectory. [default: red]
  --color_2=<color2>      The color of the first trajectory. [default: black]
  --color_3=<color3>      The color of the first trajectory. [default: blue]

"""

from docopt import docopt
import numpy as np
import pandas as pd
import matplotlib as mpl
import matplotlib.pyplot as plt
import yaml

# MPL settings.
mpl.rcParams['mathtext.fontset'] = 'cm'
mpl.rcParams['font.family'] = 'Latin Modern Roman'
plt.rcParams['figure.figsize'] = [9, 9]

# Datafile vars, for pandas
vars = [
  "time",
  "V1",
  "V2",
  "V3",
  "X1",
  "X2",
  "X3",
  "El",
  "Eg"
]

def plot_trajectory(arguments):
  config_file_name = arguments["<trajectory_config_file>"]
  output_file_name = arguments["<trajectory_output_file>"]
  font_size = int(arguments["--font_size"])
  delta = float(arguments["--delta"])
  
  # Open the trajectory config file and extract relevant data
  with open(config_file_name, "r") as file:
    config_file = yaml.safe_load(file)

  background_radius = float(config_file["background_radius"])
  background_metric = config_file["background_metric"]
  
  if arguments["--plot_radius"] == None:
    plot_radius = background_radius
  else:
    plot_radius = float(arguments["--plot_radius"])

  if background_metric == "Isotropic Schwarzschild":
    M = float(config_file["Isotropic_Schwarzschild_Settings"]["M"])
    bh_radius = 2 * M
  elif background_metric == "Kerr-Schild Kerr":
    M = float(config_file["KerrSchild_Kerr_Settings"]["M"])
    a = float(config_file["KerrSchild_Kerr_Settings"]["a"])

    if a*a > M*M:
      warn("Naked singularity detected. Drawing a unit radius horizon.")
      bh_radius = 1
    else:
      bh_radius = M + np.sqrt(M**2 - a**2)
  else:
    raise Exception("Cannot plot data due to unrecognized metric: " + background_metric)

  # mpl options
  mpl.rcParams['xtick.labelsize'] = font_size
  mpl.rcParams['ytick.labelsize'] = font_size

  data = pd.read_csv(output_file_name, delim_whitespace=True, names=vars)

  plt.close("all")

  fig, ax = plt.subplots()

  # Background
  background = plt.Circle((0, 0), background_radius, color="red", fill=False)
  ax.add_patch(background)
  
  # Trajectory
  ax.plot(data["X1"], data["X2"])

  # Ergosphere and horizons
  x = np.arange(-plot_radius, plot_radius, delta)
  y = x
  X, Y = np.meshgrid(x, y)

  if background_metric == "Isotropic Schwarzschild":
    R = np.sqrt(X**2 + Y**2)
  elif background_metric == "Kerr-Schild Kerr":
    part1 = X**2 + Y**2 - a**2
    part2 = np.abs(part1)
    
    R = np.sqrt((part1 + part2)/2)
    ergo = M * np.sqrt(2*(part1 + part2))/part2 - 1
    ax.contour(X, Y, ergo, [0.0], colors="black", linestyles="--")
    
  # Event horizon
  ax.contour(X, Y, R, [bh_radius], colors="black")

  plt.xlabel("$x$", fontsize = font_size);
  plt.ylabel("$y$", fontsize = font_size);

  plt.xlim([-plot_radius, plot_radius])
  plt.ylim([-plot_radius, plot_radius])

  plt.show()

def plot_energy(arguments):
  output_file_name = arguments["<trajectory_output_file>"]
  font_size = int(arguments["--font_size"])

  mpl.rcParams['xtick.labelsize'] = font_size
  mpl.rcParams['ytick.labelsize'] = font_size

  data = pd.read_csv(output_file_name, delim_whitespace=True, names=vars)

  plt.close("all")

  plt.xlabel("$t$", fontsize = font_size)

  if arguments["local"]:
    plt.plot(data["time"], data["El"])
    plt.ylabel("$E_l$", fontsize = font_size)

  else:
    plt.plot(data["time"], data["Eg"])
    plt.ylabel("$E_g$", fontsize = font_size)

  plt.show()

def plot_penrose(arguments):
  config_file_name = arguments["<penrose_config_file>"]
  trajectory_1 = arguments["<trajectory_1>"]
  trajectory_2 = arguments["<trajectory_2>"]
  trajectory_3 = arguments["<trajectory_3>"]
  
  color_1 = arguments["--color_1"]
  color_2 = arguments["--color_2"]
  color_3 = arguments["--color_3"]

  font_size = int(arguments["--font_size"])
  
  # Open the trajectory config file and extract relevant data
  with open(config_file_name, "r") as file:
    config_file = yaml.safe_load(file)
  
  background_radius = float(config_file["background_radius"])
  background_metric = config_file["background_metric"]

  if background_metric == "Isotropic Schwarzschild":
    M = float(config_file["Isotropic_Schwarzschild_Settings"]["M"])
    bh_radius = 2 * M
  else:
    raise Exception("Cannot plot data due to unrecognized metric: " + background_metric)

  # mpl options
  mpl.rcParams['xtick.labelsize'] = font_size
  mpl.rcParams['ytick.labelsize'] = font_size

  data_1 = pd.read_csv(trajectory_1, delim_whitespace=True, names=vars)
  data_2 = pd.read_csv(trajectory_2, delim_whitespace=True, names=vars)
  data_3 = pd.read_csv(trajectory_3, delim_whitespace=True, names=vars)

  plt.close("all")

  horizon = plt.Circle((0, 0), bh_radius, color="green", fill=False)
  background = plt.Circle((0, 0), background_radius, color="green", fill=False)

  fig, ax = plt.subplots()
  ax.add_patch(horizon)
  ax.add_patch(background)

  ax.plot(data_1["X1"], data_1["X2"], color=color_1)
  ax.plot(data_2["X1"], data_2["X2"], color=color_2)
  ax.plot(data_3["X1"], data_3["X2"], color=color_3)

  plt.xlabel("$x$", fontsize = font_size);
  plt.ylabel("$y$", fontsize = font_size);

  plt.xlim([-background_radius, background_radius])
  plt.ylim([-background_radius, background_radius])

  plt.show()

# Main
if __name__ == '__main__':
  arguments = docopt(__doc__, version="GRLensing data plotter 1.0")
  
  if arguments["trajectory"]:
    plot_trajectory(arguments)
  elif arguments["energy"]:
    plot_energy(arguments)
  elif arguments["penrose"]:
    plot_penrose(arguments)

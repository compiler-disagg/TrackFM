"""
env 0 TrackFM/CARM 
    1 Fastswap
    2 AIFM
"""
import numpy as np
import scipy.interpolate as interp
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
from matplotlib import cm
import itertools
import math
import random

class Carm2DFig:
    marker = itertools.cycle(('^', '*', '4', '.', '8', 's', '<', '>', '1', '2', '3', '4', '8', 's', 'p'))

    def  __init__(self, path, xlabel, ylabel, title, 
                  x_val_fsz, y_val_fsz,
                  x_fnt_sz, y_fnt_sz, lb_bold, 
                  ):
                self.path    = path
                plt.rc('xtick', labelsize=x_val_fsz) 
                plt.rc('ytick', labelsize=y_val_fsz)
                plt.grid(True, zorder=0, which='both', alpha=0.5)
                if (lb_bold):
                    plt.xlabel(xlabel, fontsize=x_fnt_sz)
                    plt.ylabel(ylabel, fontsize=y_fnt_sz)
                else:
                    plt.xlabel(xlabel, fontsize=x_fnt_sz)
                    plt.ylabel(ylabel, fontsize=y_fnt_sz)
    
    def plot (self, x_val, y_val, linestyle, ms_color, mkr, ms_sz): 
        plt.plot(x_val, y_val, linestyle=linestyle, lw=3, markerfacecolor=ms_color, marker = mkr, ms = ms_sz, markeredgewidth=3)
    
    def scatter (self, x_val, y_val): 
        plt.scatter(x_val, y_val, alpha=0.6, zorder=3)

    def cplot (self, x_val, y_val, linestyle): 
        plt.plot(x_val, y_val, linestyle)

    def barplot (self, x_val, y_val, w): 
        plt.bar(x_val, y_val, width = w, zorder=3, linewidth=1, edgecolor='black', alpha=0.9)

    def set_legend(self, lgnd, lgnd_col, lgnd_fnt_sz):
        plt.legend(lgnd, ncol = lgnd_col, labelspacing=0.05, fontsize=lgnd_fnt_sz)
    
    def set_limits(self, xmin, xmax, ymin, ymax):
        plt.axis([xmin, xmax, ymin, ymax])
	
    def set_y_lg_scale(self):
        plt.yscale('log')
    
    def set_x_lg_scale(self):
        plt.xscale('log')
    
    def set_y_limit(self, miny, maxy):
        plt.ylim(miny, maxy)

    def set_x_limit(self, minx, maxx):
        plt.xlim(minx, maxx)
    
    def save(self):
        plt.tight_layout()
        plt.savefig(self.path)



class Carm3DFig:

    def __init__(self, points, vals, ngridpoints,
                 xlim, ylim, 
                 xlabel, ylabel, zlabel, 
                 include_cbar,
                 azimuth, angle,
                 path,
                 title,
                 log,
                 zmax,
		 xstart,
		 ystart):

        self.path    = path
        xcoords      = np.linspace(xstart, xlim, ngridpoints)
        ycoords      = np.linspace(ystart, ylim, ngridpoints)
        plotx, ploty = np.meshgrid(xcoords, ycoords)
        plotz        = interp.griddata(points, (vals), (plotx, ploty), method='linear')
        plt.rc('xtick', labelsize=13) 
        plt.rc('ytick', labelsize=13)
        self.fig = plt.figure()
        ax = self.fig.gca(projection='3d')
        ax.set_xlabel(xlabel, fontsize=15)
        ax.set_ylabel(ylabel, fontsize=15)
        ax.set_zlabel(zlabel, fontsize=15)
        ax.set_zlim(1, zmax)
        my_col = cm.jet(np.random.rand(plotz.shape[0],plotz.shape[1]))
  
        
        if title != None:
            ax.set_title(title, fontsize=16)

        surf = ax.plot_surface(plotx, ploty, plotz, 
                               cstride=1, rstride=1, 
                               alpha=0.9, 
                               cmap='Spectral',
			       edgecolors='black',
			       #facecolors = my_col, 
                               vmin=0, vmax=np.max(vals), 
                               linewidth=0.5)

        # set perspective
        ax.view_init(azimuth, angle)

        if include_cbar == True:
            self.fig.colorbar(surf, shrink=0.5, aspect=10)

        self.fig.tight_layout()
    
    
    def save(self):
        self.fig.savefig(self.path)

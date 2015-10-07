print(__doc__)

import itertools

import numpy as np
from scipy import linalg
import matplotlib.pyplot as plt
import matplotlib as mpl

from sklearn import mixture

#reading file
with open('input.txt', 'r') as f:
    array = [[float(x) for x in line.split()] for line in f if line]

X = np.array(array)

def checkMaxClusterSize(gmm, set):
    half = (len(set) + len(set[0]) + 1) / 2
    predict = gmm.predict(set)
    maxClusterSize = np.amax(np.bincount(predict))
    if maxClusterSize >= half:
        return True
    return False

lowest_bic = np.infty
bic = []
gmms = []
n_components_range = range(1, min(7, len(X)))
cv_types = ['spherical', 'tied', 'diag', 'full']
for cv_type in cv_types:
    for n_components in n_components_range:
        # Fit a mixture of Gaussians with EM
        gmm = mixture.GMM(n_components=n_components, covariance_type=cv_type)
        gmm.fit(X)
        bic.append(gmm.bic(X))
        gmms.append(gmm)
        if len(bic) == 1:
            lowest_bic = bic[-1]
            best_gmm = gmm
        if bic[-1] < lowest_bic and checkMaxClusterSize(gmm, X):
            lowest_bic = bic[-1]
            best_gmm = gmm


predict = best_gmm.predict(X)
maxClusterSize = np.amax(np.bincount(predict))
print('Points in maximal cluster: ' + str(maxClusterSize))
print('Number of all points: ' + str(len(X)))
with open('numOfPoints.txt', 'w') as f:
    f.write(str(maxClusterSize/len(X)))

bic = np.array(bic)
color_iter = itertools.cycle(['k', 'r', 'g', 'b', 'c', 'm', 'y'])
clf = best_gmm
bars = []

def writeMaxClusterToFile(set, predict):
    maxCluster = np.argmax(np.bincount(predict))
    with open('output.txt', 'w') as f:
        for i in range(len(set)):
            if predict[i] == maxCluster:
                f.write(str(set[i][0]) + ' ' + str(set[i][1]) + '\n')


# Plot the BIC scores
# spl = plt.subplot(2, 1, 1)
# for i, (cv_type, color) in enumerate(zip(cv_types, color_iter)):
#     xpos = np.array(n_components_range) + .2 * (i - 2)
#     bars.append(plt.bar(xpos, bic[i * len(n_components_range):
#                                   (i + 1) * len(n_components_range)],
#                         width=.2, color=color))
# plt.xticks(n_components_range)
# plt.ylim([bic.min() * 1.01 - .01 * bic.max(), bic.max()])
# plt.title('BIC score per model')
# xpos = np.mod(bic.argmin(), len(n_components_range)) + .65 +\
#     .2 * np.floor(bic.argmin() / len(n_components_range))
# plt.text(xpos, bic.min() * 0.97 + .03 * bic.max(), '*', fontsize=14)
# spl.set_xlabel('Number of components')
# spl.legend([b[0] for b in bars], cv_types)

# Plot the winner
splot = plt.subplot()
Y_ = clf.predict(X)

for i, (mean, color) in enumerate(zip(clf.means_,
                                             color_iter)):
    #v, w = linalg.eigh(covar)
    v, w = linalg.eigh(clf._get_covars()[i][:2, :2])
    u = w[0] / np.linalg.norm(w[0])
    angle = np.arctan2(u[1], u[0])
    angle = 180 * angle / np.pi  # convert to degrees
    ell = mpl.patches.Ellipse(clf.means_[i, :2], v[0], v[1],
                                  180 + angle, color=color)
    if not np.any(Y_ == i):
        continue
    plt.scatter(X[Y_ == i, 0], X[Y_ == i, 1], 10, color=color)

    # Plot an ellipse to show the Gaussian component
    # angle = np.arctan2(w[0][1], w[0][0])
    # angle = 180 * angle / np.pi  # convert to degrees
    # v *= 4
    # ell = mpl.patches.Ellipse(mean, v[0], v[1], 180 + angle, color=color)
    ell.set_clip_box(splot.bbox)
    ell.set_alpha(.5)
    splot.add_artist(ell)

writeMaxClusterToFile(X, Y_)

min_x = X.min(axis=0)[0] - 1
max_x =  X.max(axis=0)[0] + 1
min_y = X.min(axis=0)[1] - 1
max_y = X.max(axis=0)[1] + 1
plt.xlim(min_x, max_x)
plt.ylim(min_y, max_y)
plt.xticks(np.arange(min_x - 1, max_x+1, 1.0))
plt.yticks(np.arange(min_y - 1, max_y+1, 1.0))
plt.title('Selected GMM: full model, %d components' % best_gmm.get_params()['n_components'])
plt.subplots_adjust(hspace=.35, bottom=.02)
plt.show()
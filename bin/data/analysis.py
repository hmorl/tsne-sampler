# Audio analysis and t-SNE part
# Based on Gene Kogan's audio-tsne.ipynb in the ml4a guides.

from matplotlib import pyplot as plt
import fnmatch
import os
import numpy as np
import librosa
import matplotlib.pyplot as plt
from sklearn.manifold import TSNE
import time
import random
import json

# Start timing the script
tStart = time.perf_counter()

# Path of wavs
path = os.getcwd() + '/wavs'

files = []
# Look for .wav files (recursively) in the given folder
for root, dirnames, filenames in os.walk(path):
    for filename in fnmatch.filter(filenames, '*.wav'):
        files.append(os.path.join(root, filename))

print("Found %d .wav files in %s"%(len(files),path))

# Define the get_features function which returns a feature vector of 39 features
def get_features(y, sr):
    y = y[0:sr]  # analyze just first second
    S = librosa.feature.melspectrogram(y, sr=sr, n_mels=128)
    log_S = librosa.amplitude_to_db(S, ref=np.max)
    # 13 MFCCs
    mfcc = librosa.feature.mfcc(S=log_S, n_mfcc=13)
    # 13 1st-order deltas
    delta_mfcc = librosa.feature.delta(mfcc, mode='nearest')
    # 13 2nd-order deltas
    delta2_mfcc = librosa.feature.delta(mfcc, order=2, mode='nearest')
    # Normalize vector
    feature_vector = np.concatenate((np.mean(mfcc,1), np.mean(delta_mfcc,1), np.mean(delta2_mfcc,1)))
    feature_vector = (feature_vector-np.mean(feature_vector)) / np.std(feature_vector)
    return feature_vector

feature_vectors = []
sound_paths = []

# Calculate feature vector for each file
print("Calculating feature vectors...")
for i,f in enumerate(files):
    if(i%50==0):
        print("Calculating %d of %d" % (i+1, len(files)))
    try:
        y, sr = librosa.load(f)
        if len(y) < 2:
            print("error loading %s" % f)
            continue
        feat = get_features(y, sr)
        feature_vectors.append(feat)
        sound_paths.append(f)
    except:
        print("error loading %s" % f)
        
print("Done. Running t-SNE...")

# Start timing
t0 = time.perf_counter()
# Perplexity of 30 performs well
model = TSNE(n_components=2, learning_rate=150, perplexity=30, verbose=1, angle=0.1).fit_transform(feature_vectors)

# For reference - TSNE default parameters:
# TSNE(n_components=2, perplexity=30.0, early_exaggeration=12.0,
# learning_rate=200.0, n_iter=1000, n_iter_without_progress=300,
# min_grad_norm=1e-07, metric='euclidean', init='random', verbose=0,
# random_state=None, method='barnes_hut', angle=0.5)

# End timing
t1 = time.perf_counter()

dur = (t1-t0) * 1000
print("t-SNE completed in {0:.0f} ms".format(dur))

# Get names for labelling the diagram (just for rough viewing purposes)
# Difficult to read them all on one graph but useful
# to check that all the hats, kicks, snares etc have been placed together
names = []
for p in sound_paths:
    names.append(os.path.basename(os.path.splitext(p)[0]))

x_axis=model[:,0]
y_axis=model[:,1]

# Plot scatter graph based on t-SNE model

plt.figure(figsize = (10,10))
plt.scatter(x_axis, y_axis)

# Label points
for label, x, y in zip(names, x_axis, y_axis):
    if(label=='cr'):
        col = 'red'
    else:
        col = 'blue'
    plt.annotate(
        label,
        xy=(x, y), xytext=(-20, 20),
        textcoords='offset points', ha='right', va='bottom',
        bbox=dict(boxstyle='round,pad=0.5', fc=col, alpha=0.01),
        arrowprops=dict(arrowstyle = '->', connectionstyle='arc3,rad=0'))

# ** CHANGE THIS TO SET WHERE THE JSON ENDS UP ***
tsne_path = "tsne-data.json"

# Normalize to 0 - 1 for the JSON
x_norm = (x_axis - np.min(x_axis)) / (np.max(x_axis) - np.min(x_axis))
y_norm = (y_axis - np.min(y_axis)) / (np.max(y_axis) - np.min(y_axis))

# Write data to JSON
# Changed from os.path.abspath to os.path.relpath so not computer-specific
data = [{"path":os.path.relpath(f), "point":[float(x), float(y)]} for f, x, y in zip(sound_paths, x_norm, y_norm)]
with open(tsne_path, 'w') as outfile:
    json.dump(data, outfile)

print("Saved JSON to %s" % tsne_path)

# End timing
tEnd = time.perf_counter()
print("Everything done in {0:.0f} s. Have a nice day".format(tEnd-tStart))

# Show scatter graph in new window (for a quick inspection)
plt.show()
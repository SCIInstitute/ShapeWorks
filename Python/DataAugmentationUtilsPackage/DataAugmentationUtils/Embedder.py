# Embedder Module
# Jadie Adams
# July 2020
# Embedder objects take a data matrix and embed it into a lower dimensional space
import os
import numpy as np
from abc import ABC, abstractmethod
from shapeworks.utils import sw_message
from pathlib import Path
from glob import glob

# abstract base class for embedders 
class Embedder(ABC):
	# abstract method
	def __init__(self, data_matrix):
		self.data_matrix = data_matrix
	def getEmbeddedMatrix(self):
		pass
	def project(self, PCA_instance):
		pass
 
# instance of embedder that uses PCA for dimension reduction
class PCA_Embbeder(Embedder):
    def __init__(self, data_matrix=None, num_dim=0, percent_variability=0.95):
        """
        Initialize the PCA_Embedder. If data_matrix is provided, a PCA model is generated.
        Otherwise, the attributes defining the model are initialized as None. A model can then be initialized from arrays
        using load_pca.

        Parameters
        ----------
        data_matrix
            Data to use to generate a PCA model
        num_dim
            Number of PCA dimensions to keep in the generated PCA scores. (Max is data_matrix.shape[0]-1, i.e., the maximum number of
            modes of variation is one less than the number of samples used to build the model.
            If set to zero, the maximum number of dimensions are kept.
        percent_variability
            Percentage of the variation in the input data to keep in the generated PCA scores, scaled to between 0 and 1.
            This is only used if num_dim is not set.
        """
        super().__init__(data_matrix)
        self.PCA_scores = None
        self.eigen_vectors = None
        self.eigen_values = None

        if data_matrix is not None:
            self.data_matrix = data_matrix
            self.mean_data = np.mean(self.data_matrix, axis=0)
            self.run_PCA(num_dim, percent_variability)

    def run_PCA(self, num_dim, percent_variability):
        """
        Perform principal component analysis on the data_matrix.

        Parameters
        ----------
        num_dim
            Number of PCA dimensions to keep in the model. (Max is data_matrix.shape[0]-1, i.e., the maximum number of
            modes of variation is one less than the number of samples used to build the model.
            If set to zero, the maximum number of dimensions are kept.
        percent_variability
            Percentage of the variation in the input data to keep in the model, scaled to between 0 and 1.
            This is only used if num_dim is not set.

        Returns
        -------
        num_dim
            num_dim actually used
        """
        # get covariance matrix (uses compact trick)
        N = self.data_matrix.shape[0]
        data_matrix_2d = self.data_matrix.reshape(self.data_matrix.shape[0],
                                                  -1).T  # flatten data instances and transpose
        mean = np.mean(data_matrix_2d, axis=1)
        centered_data_matrix_2d = (data_matrix_2d.T - mean).T
        trick_cov_matrix = np.dot(centered_data_matrix_2d.T, centered_data_matrix_2d) * 1.0 / np.sqrt(N - 1)
        # get eignevectors and eigenvalues
        eigen_values, eigen_vectors = np.linalg.eigh(trick_cov_matrix)
        eigen_vectors = np.dot(centered_data_matrix_2d, eigen_vectors)
        for i in range(N):
            eigen_vectors[:, i] = eigen_vectors[:, i] / np.linalg.norm(eigen_vectors[:, i])
        eigen_values = np.flip(eigen_values)
        eigen_vectors = np.flip(eigen_vectors, 1)
        # get num PCA components
        # Note that the number of the eigen_values and eigen_vectors is equal to the dimension of the data
        # matrix, but the last column is not used in the model because it describes no variation.
        cumDst = np.cumsum(eigen_values) / np.sum(eigen_values)
        if num_dim == 0:
            num_dim = np.where(cumDst >= float(percent_variability))[0][0] + 1
        W = eigen_vectors[:, :num_dim]
        PCA_scores = np.matmul(centered_data_matrix_2d.T, W)
        sw_message(f"The PCA modes of particles being retained : {num_dim}")
        sw_message(f"Variablity preserved: {str(float(cumDst[num_dim - 1]))}")

        self.PCA_scores = PCA_scores
        self.eigen_vectors = eigen_vectors
        self.eigen_values = eigen_values
        self.num_dim = num_dim
        return num_dim

    def write_PCA(self, out_dir: Path, score_option="full", suffix="txt"):
        """
        Write PCA data to a specified directory.

        Parameters
        ----------
        out_dir
            Directory in which to save PCA data
        score_option
            Option for how to save PCA scores. The full scores can be used to recreate the data used to create the
            model, which may be privileged information, so options are provided to save no information about the scores.
            Options are:
                full: Save complete scores
                Otherwise: Don't save scores
        suffix
            File extension to use
        """
        out_dir = Path(out_dir)
        if not os.path.exists(out_dir):
            os.makedirs(out_dir)
        if score_option == "full":
            np.savetxt(str(out_dir / f'original_PCA_scores.{suffix}'), self.PCA_scores)
        mean = np.mean(self.data_matrix, axis=0)
        np.savetxt(str(out_dir / f'mean.{suffix}'), mean)
        np.savetxt(str(out_dir / f'eigenvalues.txt'), self.eigen_values)
        for i in range(self.data_matrix.shape[0]):
            nm = str(out_dir / f'pcamode{i}.{suffix}')
            data = self.eigen_vectors[:, i]
            data = data.reshape(self.data_matrix.shape[1:])
            np.savetxt(nm, data)

    def project(self, PCA_instance):
        """
        Maps a given set of scores to the data values (e.g., coordinate points) they represent, given the embedded
        PCA model.

        Parameters
        ----------
        PCA_instance
            A row vector containing one score for each PCA mode.

        Returns
        -------
        data instance
            Data represented by the input scores for this PCA model

        """
        num_dim = len(PCA_instance)
        W = self.eigen_vectors[:, :num_dim].T
        data_instance = np.matmul(PCA_instance, W) + self.mean_data.reshape(-1)
        data_instance = data_instance.reshape(self.mean_data.shape)
        return data_instance

    def load_PCA(self, mean_data, eigen_values, eigen_vectors, scores=None):
        """
        Load PCA model from arrays

        Parameters
        ----------
        mean_data
        eigen_values
        eigen_vectors
        scores
        """
        self.mean_data = mean_data
        self.eigen_values = eigen_values
        self.eigen_vectors = eigen_vectors
        self.PCA_scores = scores

    @classmethod
    def from_directory(cls, directory: Path):
        """
        Factory function to create a PCA_embedder instance by loading saved data from a specified directory.

        Parameters
        ----------
        directory
            Directory from which to load data

        Returns
        -------
        embedder
            PCA_embedder instance

        """
        directory = Path(directory)

        mean = np.loadtxt(glob(str(directory / "mean*"))[0])
        eigen_values = np.loadtxt(glob(str(directory / "eigenvalues*"))[0])
        eigen_vectors = []

        eigen_vector_files = glob(str(directory / "pcamode*"))
        eigen_vector_files.sort(key=lambda f: int(str(Path(f).stem).split("pcamode")[-1]))  # Sort numerically by name
        for file in eigen_vector_files:
            eigen_vector = np.flip(np.loadtxt(file))
            eigen_vectors.append(eigen_vector.reshape((-1)))

        eigen_vectors = np.rot90(np.array(eigen_vectors))

        embedder = cls()

        scores = None
        if scores_glob := glob(str(directory / "original_PCA_scores*")):
            scores = np.loadtxt(scores_glob[0])

        embedder.load_PCA(mean, eigen_values, eigen_vectors, scores=scores)

        return embedder

    def getEmbeddedMatrix(self):
        """
        Get the embedded form of data_matrix

        Returns
        -------
        PCA_scores
            A matrix with one row for each input data sample
            The columns are floats that represent the value for each PCA mode that together represent the input data
            sample.
            The number of columns indicates the number of PCA modes that were used to generate the scores.

        """
        return self.PCA_scores

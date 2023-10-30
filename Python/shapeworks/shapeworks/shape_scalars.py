from io import BytesIO

import pandas as pd
import numpy as np
from mbpls.mbpls import MBPLS
from sklearn.model_selection import cross_val_predict
from sklearn.metrics import mean_squared_error
from matplotlib import pyplot as plt


def run_mbpls(x, y, n_components=3, cv=5):
    """ Run MBPLS on shape and scalar data """
    model = MBPLS(n_components=n_components)
    y_pred = cross_val_predict(model, x, y, cv=cv)
    mse = mean_squared_error(y, y_pred)

    prediction = cross_val_predict(model, x, y, cv=5)
    prediction = pd.DataFrame(prediction)
    y = pd.DataFrame(np.array(y))
    prediction = pd.concat((prediction, y), axis=1)
    prediction.columns = ['Predicted Scalar', 'Known Scalar']
    prediction.plot.scatter(x='Known Scalar', y='Predicted Scalar')
    plt.plot([prediction.min().min(), prediction.max().max()],
             [prediction.min().min(), prediction.max().max()], color='red')
    plt.ylabel(prediction.columns[0], fontsize=16)
    plt.xlabel(prediction.columns[1], fontsize=16)
    plt.title('MSE = {:.4f}'.format(mean_squared_error(prediction['Known Scalar'],
                                                       prediction['Predicted Scalar'])), fontsize=18);

    # return plot as PNG in memory
    figfile = BytesIO()
    plt.savefig(figfile, format='png')
    figfile.seek(0)  # rewind to beginning of file
    figdata_png = figfile.getvalue()  # extract string (stream of bytes)
    figfile.close()
    plt.close()

    # convert figdata_png to an array
    figdata_png = np.frombuffer(figdata_png, dtype=np.uint8)

    return figdata_png, y_pred, mse


def pred_from_mbpls(x, y, new_x, n_components=3):
    """ Run MBPLS on shape and scalar data, then predict new_y from new_x """
    model = MBPLS(n_components=n_components)
    model.fit(x, y)
    y_pred = model.predict(new_x)
    return y_pred

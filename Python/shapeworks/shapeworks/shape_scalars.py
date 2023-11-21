from io import BytesIO

import pandas as pd
import numpy as np
from mbpls.mbpls import MBPLS
from sklearn.model_selection import cross_val_predict
from sklearn.metrics import mean_squared_error
from matplotlib import pyplot as plt
from shapeworks.utils import sw_message
from shapeworks.utils import sw_progress
from shapeworks.utils import sw_check_abort


def get_fig_png():
    # return plot as PNG in memory
    figfile = BytesIO()
    plt.savefig(figfile, format='png')
    figfile.seek(0)  # rewind to beginning of file
    figdata_png = figfile.getvalue()  # extract string (stream of bytes)
    figfile.close()
    plt.close()

    # convert figdata_png to an array
    figdata_png = np.frombuffer(figdata_png, dtype=np.uint8)
    return figdata_png


def run_mbpls(x, y, n_components=3, cv=5):
    """ Run MBPLS on shape and scalar data """

    # don't set cv higher than the number of samples
    cv = min(cv, len(x))

    global mbpls_model
    mbpls_model = MBPLS(n_components=n_components)
    if cv != 1:
        y_pred = cross_val_predict(mbpls_model, x, y, cv=cv)

    mbpls_model.fit(x, y)
    
    if cv == 1:
        y_pred = mbpls_model.predict(x)

    mse = mean_squared_error(y, y_pred)

    sw_message(f'Python MSE: {mse}')

    prediction = pd.DataFrame(np.array(y_pred))

    # concatenate all columns into one
    prediction = pd.DataFrame(prediction.values.flatten())

    y = pd.DataFrame(np.array(y))
    y = pd.DataFrame(y.values.flatten())

    prediction = pd.concat((prediction, y), axis=1)
    prediction.columns = ['Predicted Scalar', 'Known Scalar']
    prediction.plot.scatter(x='Known Scalar', y='Predicted Scalar')
    plt.plot([prediction.min().min(), prediction.max().max()],
             [prediction.min().min(), prediction.max().max()], color='red')
    plt.ylabel(prediction.columns[0], fontsize=16)
    plt.xlabel(prediction.columns[1], fontsize=16)
    plt.title('MSE = {:.4f}'.format(mean_squared_error(prediction['Known Scalar'],
                                                       prediction['Predicted Scalar'])), fontsize=18);

    # convert figdata_png to an array
    figdata_png = get_fig_png()

    return figdata_png, y_pred, mse


def run_find_num_components(x, y, max_components, cv=5):
    """ Run MBPLS on shape and scalar data to determine the number of components to use"""
    MSEs = []
    for lv in range(1, max_components):
        model = MBPLS(n_components=lv + 1)
        prediction = cross_val_predict(model, x, y, cv=cv)
        prediction = pd.DataFrame(prediction)
        MSEs.append(mean_squared_error(prediction, y))

    plt.plot(np.arange(1, max_components), MSEs)
    plt.xlabel('number of LVs', fontsize=16)
    plt.xticks(np.arange(1, max_components), np.arange(1, max_components))
    plt.ylabel('LOO-CV MSE', fontsize=16)
    plt.title('Find the right number of LVs', fontsize=18);

    # convert figdata_png to an array
    figdata_png = get_fig_png()

    return figdata_png


def pred_from_mbpls(new_x):
    """ Predict new_y from new_x using existing mbpls fit """

    if not does_mbpls_model_exist():
        sw_message('MBPLS model does not exist, returning none')
        return None

    global mbpls_model
    y_pred = mbpls_model.predict(new_x)
    # return as vector
    return y_pred.flatten()

def does_mbpls_model_exist():
    """ Check if mbpls model exists """

    # check if global variable model exists, otherwise create it
    global mbpls_model
    try:
        mbpls_model
    except NameError:
        return False

    return True

def clear_mbpls_model():
    """ Clear mbpls model """

    global mbpls_model
    try:
        mbpls_model
    except NameError:
        return

    del mbpls_model
    return
from code.modules.crosser import crossing
import code.model_processing.StringToModel as StringToModel
import numpy as np
from code.structures.Population import Population

def crossover_population(population, config):
    """
    Perform a crossing operation for random pairs of superpositions stored in the
    'population'. The number of crossovers is determined by 'crossings_number'
    Inputs:
     population         - list of Models to crossover
     config.model_generation.crossings_number   - number of required crossovers

    Outputs:
     population         - population extended with new superpositions produced by crossing

    Author: Kulunchakov Andrei, MIPT
    """
    crossings_number = int(config["model_generation"]["crossing_number"])
    # superpositions generated by crossings
    new_generated_superpositions = []
    parents = []

    for i in range(crossings_number):
        models_to_cross = np.random.choice(population, 2)
        # extract handles
        models_to_cross = [obj.handle for obj in models_to_cross]
        parents.extend(models_to_cross)
        int_pair_object = crossing(*models_to_cross)
        new_generated_superpositions.extend([int_pair_object.first, int_pair_object.second])

    # convert new strings to a population and append it to the existed one
    listOfNewModels = StringToModel.strings_to_population(new_generated_superpositions)
    for (ind, model) in enumerate(listOfNewModels):
        setattr(model, 'parents', [parents[ind - ind%2], parents[ind - ind%2 + 1]])
    list(map(lambda model: setattr(model, 'where_from', "cross"), listOfNewModels))

    return listOfNewModels
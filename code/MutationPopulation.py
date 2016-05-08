from code.modules.mutator import mutation
import code.StringToModel as StringToModel
import numpy as np
def mutate_population(population, mutations_number, number_of_variables):
    """
    Perform a mutation operation for superpositions stored in the 'population'.
    The number of mutations is determined by 'mutations_number'
    Inputs:
     population         - list of Models to crossover
     mutations_number   - number of required mutations
     number_of_variables - number of variables required by random_model_generation to build
        appropriate random model

    Outputs:
     populationNew      - population of new superpositions produced by mutations
    """
    mutations_number = int(mutations_number)
    # superpositions generated by crossings
    new_generated_superpositions = []
    for i in range(mutations_number):
        model_to_mutate = np.random.choice(population, 1)

        model_to_mutate = model_to_mutate[0].handle
        new_generated_superpositions.append(mutation(model_to_mutate, number_of_variables))

    # convert new strings to a population and append it to the existed one
    populationNew = StringToModel.strings_to_population(new_generated_superpositions)
    list(map(lambda model: setattr(model, 'where_from', "mutate"), populationNew))

    return populationNew
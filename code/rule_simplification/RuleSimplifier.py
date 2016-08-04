"""
Get the handle of a superposition and transform it in concise form such that the values
of the function do not change.
This procedure is performed for all models from a population


Note that for correct work of the rule simplification, we should transform a handle in
the way that subtrees of commutative nodes are ordered
Author: Kulunchakov Andrei, MIPT
"""
from code.modules.model_simplifier_by_rules import simplify_by_rules
from code.modules.model_reconstructer import model_reconstruct
import code.model_processing.DefConstructor as DefConstructor
import code.model_processing.Parametrizer as Parametrizer
import re

def rule_simplify(population, config):
    rules_filename = construct_filename(config)
    fopen = open('log.txt','w')
    for ind, model in enumerate(population):
        backup_handle = model.handle
        handle = model_reconstruct(backup_handle)
        handle = simplify_by_rules(handle, rules_filename)

        # here we fix some freaky bug
        # only the God knows the reasons of it
        while handle.find('x1') != -1:
            ind = handle.find('x1')
            handle = handle[0:ind] + 'bump_(x0)' + handle[ind+2:]

        # NOTE THAT IT CAN RUIN YOUR CLASSIFICATION MACHINE
        # STAY CAREFUL
        handle = model_reconstruct(handle)

        new_handle = handle

        population[ind].handle = handle

        if len(backup_handle) > len(new_handle):
            #print(backup_handle, '-->\n', new_handle)
            #print(model_reconstruct(backup_handle))
            setattr(population[ind], "backup_handle", backup_handle)
            population[ind].renew_tokens()
            population[ind] = Parametrizer.parametrize_model(population[ind], reparametrize = True)
            population[ind] = DefConstructor.add_def_statements_attributes(population[ind])

    return population

def construct_filename(config):
    if config["flag_type_of_processing"]["flag"] == "rules_creation":
        return config["rules_creation"]["rules_folder"] + config["rules_creation"]["rules_filename"]
    else:
        return config["rules_creation"]["rules_used_in_fitting"]

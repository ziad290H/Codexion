#include "codexion.h"

bool parsing(char **argv,t_config *config)
{

    long tmp;

    if(!ft_parse_positive_field(argv[1], 1, &tmp))
    { 
        fprintf(stderr, "number_of_coders should be positiv int");
        return (false);
    }
    config->scheduler = (int)tmp;
    
    if(!ft_parse_positive_field(argv[2], 0, &config->time_to_burnout))
    {
        printf("time_to_burnout should be a positive int");
        return (false);
    }
    if (!ft_parse_positive_field(argv[3], 0, &config->time_to_compile))
    {
        printf("time_to_compile should be a positiv int");
        return(false);
    }
    if (!ft_parse_positive_field(argv[4], 0, &config->time_to_debug))
    {
        printf("time_to_debug should be a positiv int");
        return(false);
    }
    if (!ft_parse_positive_field(argv[5], 0, &config->time_to_refactor))
    {
        printf("time_to_refactor should be a positiv int");
        return(false);
    }
    if (!ft_parse_positive_field(argv[6], 0, &tmp))
    {
        printf("compiles_required should be a positiv int");
        return(false);
    }
    if (tmp > INT_MAX)
    {
        printf("compiles_required should be a valid positiv int");
        return (false);
    }
    config->compiles_required = (int)tmp;
    if (!ft_parse_positive_field(argv[7], 0, &config->dongle_cooldown))
    {
        printf("dongle_cooldown should be a positiv int");
        return(false);
    }
    if(ft_parse_scheduler(argv[9], &config->scheduler))
    {
        fprintf(stderr, "invalid scheduler (must be fifo or edf)");
        return (false);
    }
    return (true);
}
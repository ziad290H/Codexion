#include "codexion.h"

bool ft_atoi(const char *str, long *out)
{
    int i;
    int result;

    if (!str || str[0] == '\0')
        return (false);
    i = 0;
    result= 0;
    
    while (str[i])
    {

        if (str[i] < '0' || str[i] > '9')
            return (false);
        if (result > (LONG_MAX- (str[i] - '0')) / 10)
            return (false);
        result = result * 10 + (str[i] - '0');
        i++;
    }
    *out = result;
    return (true);
}

bool	ft_parse_scheduler(const char *str, int *out_sched)
{
	if (!str)
		return (false);
	if (strcmp(str, "fifo") == 0)
	{
		*out_sched = 0;
		return (true);
	}
	if (strcmp(str, "edf") == 0)
	{
		*out_sched = 1;
		return (true);
	}
	return (false);
}

bool	ft_parse_positive_field(const char *str, long min_val, long *out)
{
	if (!ft_atoi(str, out))
		return (false);
	if (*out < min_val)
		return (false);
	if (*out > INT_MAX)
		return (false);
	return (true);
}
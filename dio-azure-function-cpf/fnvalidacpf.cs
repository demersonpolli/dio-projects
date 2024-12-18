using System;
using System.IO;
using System.Threading.Tasks;
using Microsoft.AspNetCore.Mvc;
using Microsoft.Azure.WebJobs;
using Microsoft.Azure.WebJobs.Extensions.Http;
using Microsoft.AspNetCore.Http;
using Microsoft.Extensions.Logging;
using Newtonsoft.Json;

namespace dio_cpf
{
    public static class fnvalidacpf
    {
        [FunctionName("fnvalidacpf")]
        public static async Task<IActionResult> Run(
            [HttpTrigger(AuthorizationLevel.Function, "post", Route = null)] HttpRequest req,
            ILogger log)
        {
            log.LogInformation("Inicializing the CPF validation.");

            string requestBody = await new StreamReader(req.Body).ReadToEndAsync();
            dynamic data = JsonConvert.DeserializeObject(requestBody);
            if (data == null)
            {
                return new BadRequestObjectResult("Por favor, informe um CPF.");
            }
            string cpf = data?.cpf;

            if (!validateCPF(cpf))            
            {
                log.LogInformation("CPF inválido.");
                return new BadRequestObjectResult("CPF inválido.");
            }

            var responseMessage = "CPF válido.";


            return new OkObjectResult(responseMessage);
        }        

        public static bool validateCPF(string cpf)
        {
            if (string.IsNullOrEmpty(cpf))
                return false;

            cpf = cpf.Trim().Replace(".", "").Replace("-", "");

            if (cpf.Length != 11)
                return false;
            
            int[] multiplier1 = {10, 9, 8, 7, 6, 5, 4, 3, 2};
            int[] multiplier2 = {11, 10, 9, 8, 7, 6, 5, 4, 3, 2};

            string tempCPF = cpf[..9];
            int sum = 0;

            for (int i = 0; i < 9; i++)
            {
                sum += int.Parse(tempCPF[i].ToString()) * multiplier1[i];
            }

            int rest = sum % 11;
            rest = rest < 2 ? 0 : 11 - rest;

            string digit = rest.ToString();
            tempCPF += digit;
            sum = 0;

            for (int i = 0; i < 10; i++)
            {
                sum += int.Parse(tempCPF[i].ToString()) * multiplier2[i];
            }

            rest = sum % 11;
            rest = rest < 2 ? 0 : 11 - rest;

            digit += rest.ToString();

            return cpf.EndsWith(digit);
        }
    }
}

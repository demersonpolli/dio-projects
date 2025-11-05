# Exemplo de aplicação do Azure Foundry

## Definição dos modelos

* **gpt-4o** com a configuração dada em [chat-setup.json](chat-setup.json);
* **text-embeddings-large** que buscam dados em um *Azure Search Index* baseado nos seguintes artigos:

    * CHALLU, C., OLIVARES, K., ORESHKIN, B., GARZA, F., MERGENTHALER-CANSECO, M., DUBRAWSKI, A. *N-HiTS: Neural Hierarchical Interpolation for Time Series Forecasting*. arXiv, 2022. Disponível em [arXiv:2201.12886v6](https://arxiv.org/abs/2201.12886).

    * LIM, B., ARIK, S., LOEFF, N., PFISTER, T. *Temporal Fusion Transformers for Interpretable Multi-horizon Time Series Forecasting*. arXiv, 2020. Disponível em [arXiv:1912.09363v3](https://arxiv.org/abs/1912.09363).

    * ORESHKIN, B., CARPOV, D. CHAPADOS, N., BENGIO, Y. *N-BEATS: Neural Basis Expansion Analysis for Interpretable Time Series Forecasting*. arXiv, 2019. Disponível em [arXiv:1905.10437v4](https://arxiv.org/abs/1905.10437).

    * WANG, S., WU, H., SHI, X., HU, T., LUO, H., MA, L., ZHANG, J., ZHOU, J. *TimeMixer: Decomposable Multiscale Mixing for Time Series Forecasting*. arXiv, 2024. Disponível em [arXiv:2405.14616v1](https://arxiv.org/abs/2405.14616).

## Exemplo de execução no playground

*Explain me the time series models in the data*

> The retrieved documents discuss several time series models and methods for forecasting. Here's a summary of the key models and approaches mentioned:
> 
> 1. **Deep Neural Networks (DNNs) for Multi-Horizon Forecasting:**
> These models have shown strong performance improvements over traditional time series models. They often use architectures like recurrent neural networks (RNNs) and attention-based methods, including Transformer-based models. These models are designed to handle complex temporal relationships and heterogeneous data inputs 1 2 .
>
> 2. **Temporal Fusion Transformer (TFT):**
> This is an attention-based deep learning architecture tailored for multi-horizon forecasting. It incorporates static covariate encoders, gating mechanisms, sequence-to-sequence layers, and a temporal self-attention decoder. These components enable the model to handle a wide range of temporal relationships and provide interpretability for forecasting tasks 2 .
>
> 3. **N-BEATS (Neural Basis Expansion Analysis for Time Series):**
> A deep learning model specifically designed for time series forecasting. It avoids manual feature engineering and incorporates a fully connected architecture. N-BEATS has shown state-of-the-art performance on several benchmark datasets like M3 and M4, outperforming statistical and hybrid methods 3 4 .
>
> 4. **Transformer-Based Models:**
> Models like Autoformer, Informer, and LogTrans are mentioned as efficient transformer-based architectures. They often use techniques such as multi-head attention to capture long-term dependencies in time series data 5 .
>
> 5. **Traditional Statistical Models:**
> Statistical methods such as the Theta method, exponential smoothing, ARIMA, and variations are also highlighted as benchmarks in several forecasting competitions (e.g., M3 and M4). These methods have been foundational in time series analysis 3 4 .
> 
> 6. **Hybrid Models:**
> Hybrid approaches combine statistical methods with machine learning techniques, such as exponential smoothing with recurrent neural networks. These methods leverage the strengths of both domains to improve forecasting accuracy 4 .
>
>  **Computational Considerations:**
> Complexity analysis of models like N-BEATS and N-HiTS is provided, emphasizing the trade-offs between expressivity and computational cost. For example, N-HiTS reduces parameter usage while maintaining model expressivity by specializing in different frequency components 5 .
> 
> Each of these models is suitable for different types of time series forecasting tasks, depending on the complexity of the data, the presence of exogenous variables, and the desired level of interpretability. Let me know if you would like a deeper explanation of any specific model!
> 
> 5 references
> 11912.09363v3.pdf - Part 1
> 21912.09363v3.pdf - Part 2
> 31905.10437v4.pdf - Part 1
> 41905.10437v4.pdf - Part 2
> 52201.12886v6.pdf - Part 1


*What model would you recomend for a time series on monthly steel sales?*

> Based on the retrieved documents, a suitable model for monthly steel sales forecasting could depend on the complexity of the data, the presence of seasonality, and the desired level of interpretability. Here are some recommendations:
>
> 1. **N-BEATS (Neural Basis Expansion Analysis for Time Series):**
> N-BEATS is a deep learning model specifically designed for time series forecasting. It avoids manual feature engineering and uses a fully connected architecture. It has demonstrated state-of-the-art performance in forecasting competitions like M4, which includes monthly datasets 1 .
>
> 2. **Transformer-Based Models:**
> Models like FEDformer and Informer are efficient transformer-based architectures tailored for long-term time series forecasting. These models excel in capturing complex temporal relationships and could handle monthly data effectively 2 .
>
> 3. **Statistical Models (Theta Method, ARIMA, or ETS):**
> If the steel sales data exhibit strong seasonality or trends, traditional statistical models like the Theta method or exponential smoothing could be effective. These methods have also performed well in forecasting competitions such as M3 1 .
>
> 4. **Hybrid Models:**
> A hybrid approach combining statistical methods with machine learning techniques (e.g., exponential smoothing with recurrent neural networks) could be beneficial for leveraging the strengths of both domains 1 .
>
> 5. **Temporal Fusion Transformer (TFT):**
> This model is tailored for multi-horizon forecasting and incorporates attention-based mechanisms to handle heterogeneous data inputs, including static covariates and time-dependent variables. It could be useful for monthly steel sales if there are additional covariates like economic indicators or pricing data 3 .
> 
> The choice of model depends on the specific characteristics of your dataset (e.g., presence of exogenous variables, non-linear patterns) and computational resources. If you can provide more details about the dataset, I can refine the recommendation further!
>
> 3 references
> 11905.10437v4.pdf - Part 1
> 22405.14616v1 (1).pdf - Part 1
> 31912.09363v3.pdf - Part 1

*Would you recoment DeepAR model to a CPU utilization time series?*

> The requested information is not available in the retrieved data. Please try another query or topic.

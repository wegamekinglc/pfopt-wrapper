#include <boost/chrono.hpp>
#include <coin/IpIpoptApplication.hpp>
#include <pfopt/utilities.hpp>
#include <pfopt/meanvariance.hpp>
#include <iomanip>

#include "net_dxzq_multifactor_optimizer_IpOptimizer.h"

/*
* Class:     net_dxzq_multifactor_optimizer_IpOptimizer
* Method:    createQPandCalc
* Signature: (I[[D[D[D[DLjava/util/List;Ljava/util/List;I[D)Z
*/

JNIEXPORT jboolean JNICALL Java_net_dxzq_multifactor_optimizer_IpOptimizer_createQPandCalc
(JNIEnv * env, jobject obj_this, jint nbOfVar, jobjectArray CMatrix, jdoubleArray RMatrix, jdoubleArray LowerBnd, jdoubleArray UperBnd, jobject industryCodesGroups, jobject industryWeights,jint isEnableCsvLog, jdoubleArray Weights)
{
	std::cout << "Enter IpOptimizer_createNPandCalc." << std::endl;
	MatrixXd varMatrix(nbOfVar, nbOfVar);
	jarray myarray;
	//double cmatrix[nbOfVar][nbOfVar]; ///////////////////// Get C Matrix//////////////////////
	for (int i = 0; i < nbOfVar; i++)
	{
		myarray = (jarray)((env)->GetObjectArrayElement(CMatrix, i));
		double *coldata = (env)->GetDoubleArrayElements((jdoubleArray)myarray, 0);
		for (int j = 0; j < nbOfVar; j++)
		{
			varMatrix(i,j) = coldata[j];
		}
		(env)->ReleaseDoubleArrayElements((jdoubleArray)myarray, coldata, 0);
	}
	

	/////////////get Returns Matrix/////////////////////

	VectorXd expectReturn(nbOfVar);
	double *coldata = (env)->GetDoubleArrayElements((jdoubleArray)RMatrix, 0);
	for (int j = 0; j < nbOfVar; j++)
	{
		expectReturn(j) = coldata[j];
	}

	

	int variableNumber = nbOfVar;

	/////////////get lower bound Matrix/////////////////////

	VectorXd bndl(nbOfVar);
	coldata = (env)->GetDoubleArrayElements((jdoubleArray)LowerBnd, 0);
	for (int i = 0; i != nbOfVar; ++i)
		bndl[i] = coldata[i];


	
	/////////////get upper bound Matrix/////////////////////

	VectorXd bndu(nbOfVar);
	coldata = (env)->GetDoubleArrayElements((jdoubleArray)UperBnd, 0);

	for (int i = 0; i != nbOfVar; ++i)
			bndu[i] = coldata[i];



	/////////////get Industry Weights Matrix/////////////////////
//	std::cout << "Begin to process Industry Constrain Weights list." << std::endl;

	jclass listClass = env->FindClass("java/util/List");
	jmethodID list_get = env->GetMethodID(listClass, "get", "(I)Ljava/lang/Object;");
	if (list_get == nullptr)
		std::cout << ("Can't get MethodID for \"java.util.List.get(int location)\".\n");
	jmethodID list_size = env->GetMethodID(listClass, "size", "()I");
	if (list_size == nullptr)
		std::cout << ("Can't get MethodID for \"int java.util.List.size()\".\n");

	int weightsCount;// = env->GetArrayLength((jobjectArray)industryWeights);
	weightsCount = (int)env->CallIntMethod(industryWeights, list_size);
//	std::cout << "Size of Industry Constrain industryWeights list is " << weightsCount << std::endl;	

	VectorXd weightConstrains(weightsCount);
	jclass jcDouble = env->FindClass("java/lang/Double");
	jmethodID jmidDoubleValue = env->GetMethodID(jcDouble, "doubleValue", "()D");
	for (int i = 0; i < weightsCount; i++)
	{
		jobject weightsObject = env->CallObjectMethod(industryWeights, list_get, i);		
		double dweight = env->CallDoubleMethod(weightsObject, jmidDoubleValue);
		weightConstrains(i) = dweight;
		std::cout << "Wight index: " << i << ":" << dweight << std::endl;
	}

	/////////////get Industry Constrain Matrix/////////////////////
//	std::cout << "Begin to process Industry Constrain Matrix." << std::endl;
	int srcCount = (int)env->CallIntMethod(industryCodesGroups, list_size);

	std::cout << "Sizeof industryCodesGroups:" << srcCount << std::endl;

	std::list<VectorXi> indConstrainList;
	for (int i = 0; i < srcCount; i++)
	{
	//	std::cout << "Begin to process Industry Constrain Matrix." << std::endl;

		jobject cur_group = env->CallObjectMethod(industryCodesGroups, list_get, i);

		jclass ArrayList_class = env->FindClass("java/util/ArrayList");
		jmethodID Arraylist_get = env->GetMethodID(ArrayList_class, "get", "(I)Ljava/lang/Object;");
		if (Arraylist_get == nullptr)
			std::cout << ("Can't get MethodID for \"java.util.List.get(int location)\".\n");
		jmethodID Arraylist_size = env->GetMethodID(ArrayList_class, "size", "()I");
		if (Arraylist_size == nullptr)
			std::cout << ("Can't get MethodID for \"int java.util.List.size()\".\n");


		int sizeOfIndGroup = env->CallIntMethod(cur_group, Arraylist_size);
	//	std::cout <<"Index "<< i<< " jintArray in indexGroup size is" << sizeOfIndGroup << std::endl;

		VectorXi indCodeIndexs(sizeOfIndGroup);
		jclass jcInteger = env->FindClass("java/lang/Integer");
		jmethodID jmiIntegerValue = env->GetMethodID(jcInteger, "intValue", "()I");
		for (int j = 0; j < sizeOfIndGroup; j++)
		{
			jobject indexObj =env->CallObjectMethod(cur_group, Arraylist_get, j);
			int index = env->CallIntMethod(indexObj, jmiIntegerValue);
			indCodeIndexs(j) = index;
	//		std::cout << "IndGroup: " << j << ":" << index << std::endl;

		}
		indConstrainList.push_back(indCodeIndexs);
	
	}

	/// Print the weightConstrains  Ax==b;  Gx<=h

	if (isEnableCsvLog == 1)
	{
		std::string fileNamevarMatrix = "log/varMatrix.csv";
		pfopt::io::write_csv( fileNamevarMatrix, varMatrix);

		std::string fileNameexpectReturn = "log/expectReturn.csv";
		pfopt::io::write_csv( fileNameexpectReturn, expectReturn);

		std::string fileNamebndl = "log/bndl.csv";
		pfopt::io::write_csv( fileNamebndl, bndl);

		std::string fileNamebndu = "log/bndu.csv";
		pfopt::io::write_csv( fileNamebndu, bndu);

		MatrixXd A(weightsCount + 1, nbOfVar);
		for (int i = 0; i < weightsCount + 1; i++)
		{

			for (int j = 0; j < nbOfVar; j++)
			{
				if (i == 0)
				{
					A(i, j) = 1;
				}
				else
					A(i, j) = 0;
			}
			if (i == 0)
				continue;

			std::list<VectorXi>::iterator iter = indConstrainList.begin();
			advance(iter, i - 1);
			VectorXi indCodeIndexs = *iter;
			for (int j = 0; j < indCodeIndexs.rows(); j++)
			{
				A(i, indCodeIndexs(j)) = 1;
			}
		}
		std::string fileAname = "log/A.csv";
		pfopt::io::write_csv( fileAname, A);


		VectorXd b(weightsCount + 1);
		b(0) = 1;
		for (int i = 0; i < weightConstrains.rows(); i++)
			b(i + 1) = weightConstrains(i);
		std::string fileNameweightConstrains = "log/b.csv";
		pfopt::io::write_csv( fileNameweightConstrains, b);
	}

	///end Print

	//// To Call the optimizer///////////////////////////////////



	VectorXd expectReturn_sub = expectReturn.block(0, 0, nbOfVar, 1);
	MatrixXd varMatrix_sub = varMatrix.block(0, 0, nbOfVar, nbOfVar);
	Ipopt::SmartPtr<pfopt::MeanVariance> mynlp = new pfopt::MeanVariance(expectReturn_sub, varMatrix_sub, weightsCount + 1, indConstrainList, weightConstrains);
	mynlp->setBoundedConstraint(bndl, bndu);
	Ipopt::SmartPtr<Ipopt::IpoptApplication> app = IpoptApplicationFactory();
	app->Options()->SetNumericValue("tol", 1e-8);
	app->Options()->SetIntegerValue("print_level", 5);
	app->Options()->SetStringValue("linear_solver", "ma27");
	app->Options()->SetStringValue("hessian_approximation", "limited-memory");
	Ipopt::ApplicationReturnStatus status = app->Initialize();

	std::cout << "Start to call OptimizeTNLP." << std::endl;

	status = app->OptimizeTNLP(mynlp);
	std::cout << status << std::endl;
	std::cout << "End to call OptimizeTNLP." << std::endl;


	coldata = (env)->GetDoubleArrayElements((jdoubleArray)Weights, 0);


	VectorXd weights(nbOfVar);
	auto wts = mynlp->xValue();
	if (status == Ipopt::Solve_Succeeded || status == Ipopt::Solved_To_Acceptable_Level)
	{		
		std::cout << "OptimizeTNLP Success!" << status << std::endl;

		for (int i = 0; i != nbOfVar; ++i)
		{
			coldata[i] = wts[i];
			weights(i) = wts[i];
		}
		(env)->ReleaseDoubleArrayElements((jdoubleArray)Weights, coldata, 0);

		if (isEnableCsvLog == 1)
		{
			std::string fileNameweights = "log/weights.csv";
			pfopt::io::write_csv(fileNameweights, weights);
		}
		return true;
	}
	else
	{
		std::cout << "OptimizeTNLP Failed!" << status << std::endl;
		return false;
	}
		

}


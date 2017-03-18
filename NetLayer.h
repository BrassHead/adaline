// NetLayer.h -- Defines some classes that represents Layers of artificial
//	 	 Neurons in perceptron and adaline neural nets
//
// Copyright (C) 1998-2013 Brian Bray, All rights reserved.
//

class NetLayer {
public:
    NetLayer(int ninputs, int nneurons)
        : m_ninputs(ninputs), m_nneurons(nneurons) {}
    virtual ~NetLayer() {}

    virtual void eval(float ins[], float outs[])=0;
    virtual void train(float ins[], float targets[])=0;
    virtual void getWeights(int ineuron, float weights[]) const =0;

    int getNumInputs() const {
        return m_ninputs;
    }
    int getNumNeurons() const {
        return m_nneurons;
    }

protected:
    int		m_ninputs;
    int		m_nneurons;
};


class Perceptron : public NetLayer {
public:
    Perceptron(int ninputs, int nneurons, float theta, float alpha=1.0);
    ~Perceptron();

    void eval(float ins[], float outs[]);
    void train(float ins[], float targets[]);
    void getWeights(int ineuron, float weights[]) const;

private:
    float	m_theta;
    float	m_alpha;
    float*	m_pMatrix;
    float*	m_pBias;
};


Perceptron::Perceptron(int ninputs, int nneurons, float theta, float alpha)
    : NetLayer(ninputs, nneurons),
      m_theta( theta ),
      m_alpha( alpha )
{
    m_pMatrix = new float[ninputs*nneurons];
    m_pBias = new float[nneurons];

    for (int i=0; i<ninputs*nneurons; i++)
        m_pMatrix[i] = 0.0;
    for (int i=0; i<nneurons; i++)
        m_pBias[i] = 0.0;
}

Perceptron::~Perceptron()
{
    delete [] m_pMatrix;
    delete [] m_pBias;
}

void Perceptron::eval(float ins[], float outs[])
{
    float accum;
    for (int i=0; i<m_nneurons; i++) {
        int mloc = i*m_ninputs;
        accum = m_pBias[i];

        for (int j=0; j<m_ninputs; j++) {
            accum += ins[j]*m_pMatrix[mloc+j];
        }
        if (accum<-m_theta)
            outs[i] = -1.0;
        else if (accum>m_theta)
            outs[i] = +1.0;
        else
            outs[i] = 0.0;
    }
}

void Perceptron::train(float ins[], float targets[])
{
    float* resp = new float[m_nneurons];

    eval(ins,resp);

    for (int i=0; i<m_nneurons; i++) {
        if (resp[i]!=targets[i]) {
            int mloc = i*m_ninputs;
            m_pBias[i] += m_alpha*targets[i];

            for (int j=0; j<m_ninputs; j++) {
                m_pMatrix[mloc+j] += m_alpha*targets[i]*ins[j];
            }
        }
    }
}

void Perceptron::getWeights(int ineuron, float weights[]) const
{
    int mloc = ineuron*m_ninputs;
    for (int i=0; i<m_ninputs; i++)
        weights[i] = m_pMatrix[mloc+i];
}



class Adaline : public NetLayer {
public:
    Adaline(int ninputs, int nneurons, float alpha=0.1);
    ~Adaline();

    void eval(float ins[], float outs[]);
    void train(float ins[], float targets[]);
    void getWeights(int ineuron, float weights[]) const;

private:
    float	m_alpha;
    float*	m_pMatrix;
    float*	m_pBias;
};



Adaline::Adaline(int ninputs, int nneurons, float alpha)
    : NetLayer(ninputs, nneurons),
      m_alpha( alpha )
{
    m_pMatrix = new float[ninputs*nneurons];
    m_pBias = new float[nneurons];

    for (int i=0; i<ninputs*nneurons; i++)
        m_pMatrix[i] = (float)rand()/(float)RAND_MAX * 5.0 - 2.5;
    for (int i=0; i<nneurons; i++)
        m_pBias[i] = (float)rand()/(float)RAND_MAX * 5.0 - 2.5;
}

Adaline::~Adaline()
{
    delete [] m_pMatrix;
    delete [] m_pBias;
}

void Adaline::eval(float ins[], float outs[])
{
    float accum;
    for (int i=0; i<m_nneurons; i++) {
        int mloc = i*m_ninputs;
        accum = m_pBias[i];

        for (int j=0; j<m_ninputs; j++) {
            accum += ins[j]*m_pMatrix[mloc+j];
        }
        outs[i] = accum;
    }
}

void Adaline::train(float ins[], float targets[])
{
    float* resp = new float[m_nneurons];

    eval(ins,resp);

    for (int i=0; i<m_nneurons; i++) {
        if (resp[i]!=targets[i]) {
            int mloc = i*m_ninputs;
            m_pBias[i] += m_alpha*(targets[i]-resp[i]);

            for (int j=0; j<m_ninputs; j++) {
                m_pMatrix[mloc+j] += m_alpha*(targets[i]-resp[i])*ins[j];
            }
        }
    }
}

void Adaline::getWeights(int ineuron, float weights[]) const
{
    int mloc = ineuron*m_ninputs;
    for (int i=0; i<m_ninputs; i++)
        weights[i] = m_pMatrix[mloc+i];
}



